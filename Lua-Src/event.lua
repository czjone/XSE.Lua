---事件对象
--@type eventdispather
eventdispather  = {}

---@type name
eventCacheItem = {}

---全局的事件管理器
_G_eventCache = _G_eventCache or {} ---type,key,fun

--------------------------------------------------------------------
---事件管理器
--
---事件派发共用函数
local function dispatch(listener,args)
    assert(listener and listener.getEventType,"in class listener not found function \"getEventType\"")
    --迭代派发事件到指定的对象
    table.foreach(_G_eventCache,function(k,v)
        if listener:getEventType() == v.id then
            if listener.eventAction ~= nil then
                listener:eventAction(v.callBack,args)
            elseif v.callBack ~=nil  then
                v.callBack(listener,args)
            end
            warning(false,"listener does not implement event handlers eventAction")
        end
    end)
end

---添加事件
local function add(listener,fun,key_)
    local id_ = listener:getEventType()
    table.insert(_G_eventCache,{id = id_,#(_G_eventCache) + 1,key = key_,callBack = fun });
end

---移除事件
local function remove(listener,key_)
    table.removeA(_G_eventCache,function(v)
        return v.id == listener:getEventType() and  v.key == key_
    end,true)
end


--------------------------------------------------------------------------
--事件对象派发器

---添加事件监听 PS:fun的参数个数将由 dispatchProcess 重写的情况来定
function eventdispather:addEventListener(fun ,key_)
    add(self,fun,key_)
end


---移除事件监听
function eventdispather:removeEventListener(key)
    return remove(self,key)
end

---派发事件,禁止重写本方法，只可调用
function eventdispather:dispatch(args)
    return dispatch(self,args)
end

---
-- @function [parent=#eventdispather] dispatchProcess
-- @param self
-- @param fun
-- @param args
-- @usage 在派生类中得写这个函数
--function dispatchProcess(fun,args)
--    ---fun的调用模式会决定function的事件参数类型
--    xpcall(fun(self,args),function() --处理逻辑 end)
--end
-- @return nil

---设置当前事件的类型
function eventdispather:getEventType()
    if self.____id__ == nil then
        self.____id__ = xse.base.timer.time()
    end
    return self.____id__
end

---------------------------------------------------------------
--事件抽象函数

--- @field [parent=#global] event
event  = {}

function event:ctor()

end

function event:eventAction()

end

return eventdispather
