#Attempts to find cononical AMD64 kernel mode addresses within files
import sys
import binascii

if len(sys.argv) < 2:
	print 'target file required'
	sys.exit(1)
	
file = sys.argv[1]
with open(file, 'rb') as f:
	data = bytearray(f.read())
for index, byte in enumerate(data):
	if byte == 0xFF:
		if data[index - 1] == 0xFF:
			if data[index - 2] >= 0x80:
				out = []
				for i in range(8):
					out.append(data[index - i])
				print 'Possibly a little endian kernel mode pointer: 0x' + binascii.hexlify(bytearray(out))