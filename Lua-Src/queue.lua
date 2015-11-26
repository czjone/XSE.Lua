queue =  {}

function queue.init(tb)
	local tb = tb or {}
	return tb
end

function queue:push(n)
	local len = #self
	self[tostring(len)] = n
end

function queue:pop()
	local len = tostring(#self)
	local n =  self[len]
	self[len] = nil
	return n
end

function queue:revert()
	local len =  #self
	local loop = math.floor( len / 2)
	for i=1,loop do
		self[i],self[len - i + 1] =self[len-i+1],self[len]
	end
end

function queue:getNode(index)
	return self[tostring(index)]
end

return queue