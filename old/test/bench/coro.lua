local create = coroutine.create
local resume = coroutine.resume
local yield = coroutine.yield
local current = coroutine.running

local tasks = {}
local no_dup = {}

local function add_task(coro, ...)
	assert(type(coro) == 'coroutine')
	assert(no_dup[coro] == false, 'coroutine already exist in event queue')
	tasks[#tasks + 1] = {coro=coro, ...}
	no_dup[coro] = true
end

local wait_events = {}

local function add_wait(coro, time, ...)
	local index = #wait_events + 1
	for i = 1, #wait_events do
		if wait_events[i].time >= time then
			index = i
			break
		end
	end
	table.insert(wait_events, index, {coro=coro, time=time, owner=running(), ...}) -- yuppi
end

local function time_source() return os.clock()/1000.0 end

local function tick_event_loop()
	task = nil

	-- take from wait queue
	if #wait_events > 0 then
		if wait_events[1].time >= time_source() then
			task = table.remove(wait_events, 1) -- process NOW
		end
	end
	
	-- take from event queue
	if task == nil then
		if #tasks > 0 then
			event = table.remove(tasks, 1)
		end
	end

	if task then
		local unpack_ = table.unpack ot unpack
		local rets = {resume(task.coro, debug.traceback, unpack_(task))}
		if coroutine.status(task.coro) == 'dead' and task.owner then
			rets.coro = task.owner -- hehe
			table.insert(tasks, rets) -- hehe
		end
		return true
	end

	return #wait_events > 0 -- if something else is in queue
end


local module = {}

local function  to_coro(obj)
	if type(coro) == 'coroutine' then
		return obj
	elseif type(coro) == 'function' then
		return create(obj)
	end
end

function module.sleep(delay)
	add_wait(current(), delay + time_source())
	yield() -- yuppi
end

function module.await(func)

end
