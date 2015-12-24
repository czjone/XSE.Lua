xse = {};


do -- start base block 

-------------------------------------------------------------------
-- globle functions

_G.__assert = _G.assert ;
function assert ( exp,str, ... ) _G.__assert(exp,str,...); end

_G.__log = _G.log;
function log ( str,... ) print(str,...); end

_G.debugflage = false;
xse.debugflage = false;

function _G.set_debug(debugflag)
	_G.debugflag = debugflag;
end

function _G.get_debug()
	return xse.debugflage == true or _G.debugflage == true;
end

function try( fun,onerrorcallback)
	if get_debug() == true then
		xpcall(fun,onerrorcallback);
	else
		xpcall(fun,function(errmsg)
			log(errmsg);
		end)
	end 
end

function switch(switch_condition,table_name,errcallfun)
	local fun =  table_name[switch_condition];
	assert(fun,"get function is nil");
	assert(type(fun) == "function","get val is not functionn.");
	try(function() fun() end, function(errmsg) errcallfun(errmsg); end);
end

-------------------------------------------------------------------
-- define extends object fileds.
function define_property( self,name,defaultval )
	self["__"..name] = defaultval;
	
	self["get_"..name] = function ( self )
		return self["__"..name];
	end

	self["set_"..name] = function ( self,val )
		self["__"..name] = val;
	end

	return self;
end

-------------------------------------------------------------------
-- define  extends class from class. 
local vtable = "vtable";

function extends(self,base)
	self[vtable] = self[vtable] or {};
	local tb =  self[vtable];
	table.insert(tb,#(tb),base); -- set virtual objects like c++ design.
	-- set direct access call method for call method whith base object.
	for k,v in pairs(base) do
		self[k] = self[k] or v;
	end
	return self; 
end

local function callbase(self,name,...)
	--- call base method like c++ call base method.
	assert(self,"self is nil.");
	local _vtable = self[vtable];
	assert(_vtable,"it have not a base object.");
	local _vtablecount = #(_vtable)；
	for i=1,_vtablecount do
		local baseobj = _vtable[_vtablecount - i + 1];
		local target_fun = baseobj[name];
		if target_fun ~= nil then
			try(function() target_fun(...) end,function(errmsg) log(errmsg); end);
			break;
		end
	end
	return nil;
end

-------------------------------------------------------------------
-- define base object
object = {}

define_property(object,"supper",nil);
define_property(object,"this",nil);
define_property(object,"hashcode",nil);

function object:ctor(...)
	-- body
end

function object:dector(...)
	-- body
end

function object:new( ... )
	local instance = {};
	local hashcode = os.clock(); -- use application run times to hashcode.
	instance:set_hashcode(hashcode);

	local function copy(self,source)
		for k,v in pairs(source) do
			if type(v)=="table" then
				self[k]={};
				copy(self[k],v);
			elseif  k ~= "__index" then
				self[k] = v;
			end
		end
	end

	copy(instance,self);
	instance:ctor(...);
	return instance;
end

end  -- end base block

-------------------------------------------------------------------
-- core lib defines

define_property(xse,"version","0.0.1");


-------------------------------------------------------------------
-- string extends.
xse.string = string or {}

function xse.string.format(str,...)

end

function xse.string.split(str,cutstr)

end

function xse.string.trim(str)

end

function xse.string.bytelen(str)

end

function xse.string.utflen(str)

end

function xse.string.get_utfchar(str,startindex,utflen)

end

function xse.string.get_char(str,startindex,bytelen)

end

-------------------------------------------------------------------
-- table extends.

xse.table = table or {}

function xse.table.find(key)

end

function xse.table.each(tb,callfun,breakfun)

end

return xse;