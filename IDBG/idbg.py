import idaapi
import time 
import mmap
import struct
import ctypes

def WriteToBeginningOfMmap(shared_memory, bytes_to_write): 
	shared_memory.seek(0)
	shared_memory.write(bytes_to_write)		
		

def is_key_down(v_key_code):
	return ctypes.windll.user32.GetAsyncKeyState(v_key_code) & 1<<31


class MyIDAViewWrapper(idaapi.IDAViewWrapper):
	def __init__(self, viewName, bps_shared_mem):
		idaapi.IDAViewWrapper.__init__(self, viewName)
		self.bps_shared_memory = bps_shared_mem
		self.bp_list = []
		self.is_unloading = False
	

	def Unload(self):
		"""self.is_unloading = True
		print("unloading... idbg")
		uk.unhook_and_unbind()
		WriteToBeginningOfMmap(self.bps_shared_memory, "\x00")
		self.bps_shared_memory.close()"""
		

	def OnViewCurpos(self):
		if(not(self.is_unloading) and self.bps_shared_memory[0] == 's'):
			self.Unload()
			return 
		
		#change with whatever key you want to use see:https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes
		if(is_key_down(0x4a)): 
			#note: don't rebase if you have breakpoints that are already set (since bp_list won't be updated (todo)) (maybe could store them in base 0 so no need to rebase)
			self.HandleBp(idaapi.get_screen_ea()-idaapi.get_imagebase(), idaapi.get_screen_ea())
		
	def HandleBp(self, base0_addr, addr):
		if(addr in self.bp_list):
			self.RemoveBp(base0_addr, addr)
		else:
			self.AddBp(base0_addr, addr)

	def GetWindbgResponse(self, request_type): 
		timeout = 0
		while(self.bps_shared_memory[0] == request_type  and timeout < 10): 
			time.sleep(0.05)
			timeout += 1

		if(self.bps_shared_memory[0] == 'f'):
			return False
		elif(self.bps_shared_memory[0] == 'c'):
			return True
		
		return False


	def AddBp(self, base0_addr, addr):
		print(hex(is_key_down(0x4a)), "trying to add:", hex(base0_addr))
		if(is_key_down(0x4a) == 0):
			print("not pressing J")
			return
	
		WriteToBeginningOfMmap(self.bps_shared_memory, 'a' + struct.pack('<L', base0_addr)) #todo handle 64bit 
		
		if(self.GetWindbgResponse('a')):
			self.bp_list.append(addr)
			idaapi.add_bpt(addr, 0, BPT_SOFT)
			EnableBpt(addr, True)
			print("sent ", 'a' + hex(base0_addr), "to WinDbg")
		else:
			print("Failed to add the Breakpoint on WinDbg, try Breaking before adding a Bp")

		WriteToBeginningOfMmap(self.bps_shared_memory, "\x00")

	def RemoveBp(self, base0_addr, addr):
		WriteToBeginningOfMmap(self.bps_shared_memory, 'r' + struct.pack('<L', base0_addr))
		if(self.GetWindbgResponse('r')):
			idaapi.del_bpt(addr)
			self.bp_list.remove(addr)
		WriteToBeginningOfMmap(self.bps_shared_memory, "\x00") 

	

	"""def OnViewMouseOver(self,a ,b ,c ,d ,e):
		pass
	"""

class uihook(idaapi.UI_Hooks):
	def __init__(self):
		idaapi.UI_Hooks.__init__(self)
		self.binds = []
		self.bps_shared_memory = mmap.mmap(0, 16, "Local\\breakpoints_shared_memory")
		tm = MyIDAViewWrapper("IDA View-A", self.bps_shared_memory)
		
		#todo add the already set bps to windbg here
		self.binds.append(tm)	
		tm.Bind()
	

	def current_tform_changed(self, a1, a2):
		#print "tform", idaapi.get_tform_title(a1)
		tm = MyIDAViewWrapper(idaapi.get_tform_title(a1), self.bps_shared_memory)
		if(tm not in self.binds):
			tm.Bind()
			self.binds.append(tm)
		#self.tm.Unbind()
		
	def unhook_and_unbind(self):
		for bind in self.binds:
			bind.Unbind() 
		self.unhook()
		self.bps_shared_memory.close()

  	
uk = uihook()
uk.hook()

print("police")
"""
todo: draw cur_eip ?
	  way to get the last added/deleted bp
      todo handle error 5
"""

