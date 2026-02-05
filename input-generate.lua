math.randomseed(os.time())

os.execute("mkdir -p inputs")

for x = 100, 1167 do
	local filename = string.format("inputs/input_%d.txt", x)
	local f = assert(io.open(filename, "w"))

	-- first line: x
	f:write(x, "\n")

	-- next x lines: values in [0, 3]
	for _ = 1, x do
		f:write(math.random(0, 3), "\n")
	end

	f:close()
end
