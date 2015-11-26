----------------------------------------------------------
--面向对象核心库
----------------------------------------------------------

----------------------------------------------------------
--base object
object = {}

function object:getName()
    return "unknow"
end

function object:resetHashCode()
    self.__hashcode = nil
    self:getHashCode()
end

function object:getHashCode()
    self.__hashcode = self.__hashcode or (os.time() + math.random())
    return  self.__hashcode
end

----------------------------------------------
-- create interface helper
function createInterface(object,name,func)
    warning(object[name] == nil,"Interface exists,create fail" .. name)
end

function callInsterface(object,name,...)
    local func =  object[name]
    return func(object,...)
end

function callmethod(object,name,...)
    local func =  object[name]
    return func(object,...)
end

function implementInterface(object,name,func)
    object[name] =  func
end

function createAbstract(object,name,func)
    object[name] =  function(self)
        warning(false,"not implement Abstract " .. name)
        return nil
    end
end

function createVirtualFunction(object,name,func)
    implementInterface(object,name,func)
end

function override(object,name,func)
    implementInterface(object,name,func)
end

function sealed(object)
    object.__sealed = true
end

----------------------------------------------
--oop helpers

local function addctor(class)
    assert(class,"class is nil value")
    if class.ctor == nil then
        class.ctor = function(self) end
    end
end

function registerFun_getInstance(cls)
    --    warning(cls.getInstance == false,"cls has function:getInstacnce")
    cls.__instance = nil;
    cls.getInstance =function()
        if cls.__instance == nil then
            assert(cls.new and type(cls.new)== "function","not found ctor function :new");
            cls.__instance = cls:new()
            return cls.__instance
        end
        return cls.__instance
    end
end


---向对象中添加临时数据
function setTempVariable(object,key,value)
    assert(key,"key is a nil value")
    assert(value~=nil,"value is a nil value")
    object.__tempData = object.__tempData or  {}
    object.__tempData["__"..key] = value
end

---查找对象中保存的临时数据
function getTempVariable(object,key)
    assert(object,"object is a nil value")
    assert(key,"key is a nil value")
    if object.__tempData == nil then  return nil end
    return  object.__tempData["__"..key]
end

function callSupper(instance,supperName,name,cls,...)

    if instance ~= nil then
        if instance.supper  == nil then return end
        if  instance.supper[supperName] ~=nil then
            for key, var in pairs( instance.supper[supperName]) do
                if key ==  name then
                    local fun = var
                    local returnValue,e = pcall(fun,cls or instance,...)
                    assert(e==nil,e)
                    return returnValue,true
                end
            end
        else
            for key, var in pairs(instance.supper) do
                if type(var) == "table" and key~="__index" and key ~= "__child" then
                    local returnValue,result = callSupper(var,supperName,name,instance,...)
                    if result == true then return returnValue,true end
                end
            end
        end

        if instance.__child ~=nil then
            local returnValue,result = callSupper(instance.__child,supperName,name,cls,...)
            if result == false and instance.__child.__child then
                returnValue,result = callSupper(instance.__child.__child,supperName,name,cls,...)
            end
            return returnValue,true
        end
    end
    return nil,false
end

---调用基类中的函数
function callSupperA(instance,supperName,name,...)
    return callSupper(instance,supperName,name,instance,...)
end

local function copyAllSupper(class,base)
    if base ~= nil and class ~= nil and base.supper ~= nil then
        class.supper = class.supper or {}
        for key, var in pairs(base.supper) do
            if class.supper[key] == nil then
                class.supper[key] =  var
            end
        end
    end
end

--继承帮助函数
--ext:扩展类,
--base:基类
--supper name
---------------------------------------------
--@param #table ext 当前类的定义
--@param #table base 当前类的要继承的类，一般也为table
--@param #string suppername 当前继承的基类的别名
--@return #table 一个新的对旬的实列
function inheritanceA(ext,base,suppername)
    --    assert(ext,"not set ext object.")
    --    assert(base,"not set base object.")
    --    assert(base.__sealed == nil,"基类设置为了禁止继承.")
    if base.__sealed ~= nil then
        assert(base.__sealed == nil,"基类设置为了禁止继承.")
        return
    end

    --metatable copy
    local function setmetatableA(tag,source)
        for k,v in pairs(source) do
            if tag[k] == nil then
                if type(v) == "table" and k ~= "__index" and k~= "__child" then
                    tag[k] = {}
                    setmetatableA (tag[k],v)
                    tag[k].__index = tag[k]
                else
                    tag[k] =  v
                end
            end
        end
        return tag
    end

    ext = setmetatableA(ext,base)
    ext.__index = ext
    --添加自己的构造函数
    if ext.new == nil then
        function ext.new  ( self )
            local instance = {}
            setmetatableA(instance,self)
            instance.__index = instance
            instance:ctor()
            instance:resetHashCode()
            return instance,ext.supper
        end
        addctor(ext)
    end

    ext.supper = ext.supper or {}
    setmetatableA(ext.supper,base)
    ext.supper.__child = ext
    ext.supper.__index = ext.supper;

    ---添加基类的默认构造函数
    if ext.supper and ext.supper.new == nil then
        ---此逻辑的意义不大
        ext.supper.new =  ext.supper.new or function ( self )
            local instance = {}
            setmetatableA(instance,self)
            instance:resetHashCode()
            return ext.supper,ext.supper or {}
        end
        addctor(ext.supper)
    end

    if suppername ~= nil and type(ext.supper[suppername])~= "function" then
        ext.supper[suppername] =  ext.supper[suppername] or {}
        setmetatableA(ext.supper[suppername],base)
        ext.supper[suppername].__index = ext.supper[suppername]
    end

    ext.__index = ext;
    ---注册单列接口
    registerFun_getInstance(ext)
    return ext,ext.supper
end

---继承对象实例
--@param #table ext 当前对象的实例
--@param #table base 基类的实例
function inheritance(ext,base)
    --    assert(false,"调用本函数只能通过supper来访问，如果多少继承，只有最后一次继承的会生效")
    return inheritanceA(ext or {},base,"__supper")
end

---继承自model 文件
--@param #table ext 当前对象的实例
--@param #string mname 当前要继承的模块对象
function inheritanceM(ext,mname)
    local obj = require (mname)
    assert(obj~=nil,"load mode error,mode name:"..mname)
    return inheritanceA(ext,obj,mname)
end

function property_def(self, str_name,defaultVal)
    
    self["____"..str_name] =  defaultVal; 

    self["set_"..str_name] = function(self,val) 
        self["____"..str_name] =  val;
    end
    
    self["get_"..str_name] = function(self) 
        return self["____"..str_name];
    end
end

return object