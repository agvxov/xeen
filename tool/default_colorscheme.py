#!/bin/python3

def default_color_scheme(code : int) -> (int, int, int):
	# as defined by xterm
	if 0 <= code <= 15:
		colors = [
			(0, 0, 0),			# 0
			(205, 0, 0),		# 1
			(0, 205, 0),		# 2
			(205, 205, 0),		# 3
			(0, 0, 238),		# 4
			(205, 0, 205),		# 5
			(0, 205, 205),		# 6
			(229, 229, 229),	# 7
			(127, 127, 127),	# 8
			(255, 0, 0),		# 9
			(0, 255, 0),		# 10
			(255, 255, 0),		# 11
			(92, 92, 255),		# 12
			(255, 0, 255),		# 13
			(0, 255, 255),		# 14
			(255, 255, 255),	# 15
		]
		return colors[code]
	elif 16 <= code <= 231:
		code -= 16
		r = (code // 36) % 6
		g = (code // 6) % 6
		b = code % 6
		return (r * 40 + 55 if r else 0,
				g * 40 + 55 if g else 0,
				b * 40 + 55 if b else 0)
	elif 232 <= code <= 255:
		level = (code - 232) * 10 + 8
		return (level, level, level)

def rgb2escape(rgb : (int, int, int)) -> str:
    return "\033[38;2;{};{};{}m".format(*rgb)

#for i in range(256):
#	print(rgb2escape(default_color_scheme(i)) + "#", end='')
#	if ((i+1) % 8) == 0: print('')

for i in range(256):
	print("[{}] = 0xff{}{}{},".format(i, *["{:02x}".format(h) for h in default_color_scheme(i)]))
