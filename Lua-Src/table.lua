---@field #table table 对lua table 的扩展
table = table or  {}

--- fun interface
-- @fun function (k,v) end
function table.foreach(tb,fun)
    assert(tb,"tb is a nill value")
    for k,v in pairs(tb) do
        fun(k,v);
    end
end

---@fun fucntion(v) end
function table.findA(tb,fun )
    local k,v = table.findEnty(tb,function(k,v)
        return fun(v)
    end)
    return v
end

---@fun fucntion(k,v) end
function table.findEnty(tb,fun)
    for k,v in pairs(tb) do
        if fun(k,v) then
            return k,v
        end
    end
    return nil,nil
end

---把table2 中符合条件的数据移动table1
function table.move(tb1,tb2,funmp,rall)
    for key, var in pairs(tb2) do
        if funmp(var) then
            table.insert(tb1,var,#tb1)
            table.removeA(tb2,funmp(var),false)
        end
    end
    return tb1
end

function table.getLength(tb)
    local count = 0;
    --assert(tb,"tb is a nill value")
    for k,v in pairs(tb) do
        count =  count +1
    end
    return count
end

function table.getItemByIndex(tb,index)
    local _index = 1
    local key = nil
    table.foreach(tb,function(k,v)
        _index =  _index +1
        if index ==  _index then  key =  k end
    end)
    if key == nil then return key
    else return tb[key] end
end

---检查是否存在列
function table.exist(tb,funmp)
    if tb == nil then return false end
    for key, var in pairs(tb) do
        if funmp(var) then
            return true
        end
    end

    return false
end

function table.removeA(tb,funmp,rall)
    for i,v in ipairs(tb) do
        if funmp(v) == true then
            table.remove(tb,i)
            if rall == true then return table.removeA(tb,funmp,rall)
            else return tb end
        end
    end
    return tb
end

function table.getCount(tb,funmp)
    local count = 0
    if tb == nil or #tb == 0 then return 0 end
    table.foreach(tb,function(k,v)
        if funmp(k,v) then
            count = count+ 1
        end
    end)
    return count
end

---table2 相对table1增加的的行，组成一个新的表
--@param #table t1 table1
--@param #table  t2 table2
function table.diff(t1,t2)
    local tb_a = {}
    table.copy(tb_a,t1)
    local tb_b = {}
    table.copy(tb_b,t2)
    local tb_c = table.mergeA(tb_a,tb_b)
    table.removeA(tb_c,function(var)
        return table.exist(tb_a,function(v) return table.equse(v,var) end)
    end)
    return tb_c
end

---exp为true时就是return 的数据
function table.filer(source,exp)
    local tb = {}
    for key, var in pairs(source) do
        if exp(var) then table.insert(tb,#tb + 1,var) end
    end
    return tb;
end


---检查两个table 是否完全相同
function table.equse(a,b)
    if table.getn(a) ~= table.getn(b) then return false end
    local res = true
    table.foreach(a,function(k,v)
        if type(v) == table and k ~="__index" then
            res = table.equse(v,b[k])
        else
            res = v == b[k]
        end
    end)
    return res
end

--function table.sort(tb,expires)
--    if tb == nil or table.getLength(tb) then return tb end
--    
--    return tb
--end

---@return #bool tb 是否为空
function table.isEmpty(tb)
    if tb == nil then return true end
    local res = true
    table.foreach(tb,function(k,v) res = false end)
    return res
end

---copy source to tag
function table.copy(tag,source)
    for key, var in pairs(source) do
        if type(var)=="table" then
            local childtb = {}
            table.copy(childtb,var)
            tag[key] = childtb
        else
            tag[key] = var
        end
    end
    return tag
    --   return _tb
end

---取出table 早的数据，从startInde 到endindex
function table.sub(tb,startIndex,endindex)
    local index = 1
    local newtb = {}
    table.foreach(tb,function(k,v)
        if index >= startIndex and  index <= endindex then
            table.insert(newtb,v)
        end
        index = index + 1
    end)
    return newtb
end

---table2 相对table1增加的的行，组成一个新的表
--@param #table t1 table1
--@param #table  t2 table2
--@param #function compr 对比函数，如果为空就按k比较
--@return #table1 增加的数据
--@return #table2 删除的数据
function table.getChanged(tb1,tb2,compr)
    return table.diff(tb1,tb2,compr),table.diff(tb2,tb1,compr)
end

---合并两个表，如果两个表同时存在，使用t2中的
function table.mergeA(t1, t2)
    local t = {}
    table.foreach(t1,function(k,v)
        if type(v) == table and k ~= "__index" then
            t[k] =  table.mergeA({},v)
        else
            t[k] = v
        end
    end)
    table.foreach(t2,function(k,v)
        if type(v) == table and k ~= "__index" then
            t[k] =  table.mergeA({},v)
        else
            t[k] = v
        end
    end)
    return t
end

table.__index = table
return table
