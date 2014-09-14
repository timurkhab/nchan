//don't edit this please, it was auto-generated

typedef struct {
  //input: keys: [],  values: [ channel_id ]
  // delete this channel and all its messages
  char *delete;

  //input:  keys: [], values: [channel_id, msg_time, msg_tag, no_msgid_order, create_channel_ttl, subscriber_channel]
  //output: result_code, msg_time, msg_tag, message, content_type,  channel_subscriber_count
  // no_msgid_order: 'FILO' for oldest message, 'FIFO' for most recent
  // create_channel_ttl - make new channel if it's absent, with ttl set to this. 0 to disable.
  // result_code can be: 200 - ok, 404 - not found, 410 - gone, 418 - not yet available
  char *get_message;

  //input:  keys: [], values: [channel_id, time, message, content_type, msg_ttl, subscriber_channel]
  //output: message_tag, channel_hash {ttl, time_last_seen, subscribers}
  char *publish;

  //input:  keys: [], values: [channel_id, status_code]
  //output: current_subscribers
  char *publish_status;

  //purposely left blank
  char *subscribe;

} nhpm_redis_lua_scripts_t;

static nhpm_redis_lua_scripts_t nhpm_rds_lua_hashes = {
  "69ba50c10aca574b2145d6ecefe72896bc8d4277",
  "c1e91ece920a6aa6049d35639ca3facf3d2095d1",
  "b92bb8b7afa329fdd377f143a5af963e17a3f6ed",
  "12ed3f03a385412690792c4544e4bbb393c2674f",
  "e1c3e421513ff2ab54cf61aa5125e7b45ee71489"
};

#define REDIS_LUA_HASH_LENGTH 40

static nhpm_redis_lua_scripts_t nhpm_rds_lua_script_names = {
  "delete",
  "get_message",
  "publish",
  "publish_status",
  "subscribe",
};

static nhpm_redis_lua_scripts_t nhpm_rds_lua_scripts = {
  //delete
  "--input: keys: [],  values: [ channel_id ]\n"
  "-- delete this channel and all its messages\n"
  "local id = ARGV[1]\n"
  "local key_msg=    'channel:msg:%s:'..id --not finished yet\n"
  "local key_channel='channel:'..id\n"
  "local messages=   'channel:messages:'..id\n"
  "local subscribers='channel:subscribers:'..id\n"
  "local pubsub=     'channel:pubsub:'..id\n"
  "local enable_debug=true\n"
  "local dbg = (function(on)\n"
  "if on then\n"
  "  return function(...)\n"
  "  redis.call('echo', table.concat({...}))\n"
  "end\n"
  "  else\n"
  "    return function(...)\n"
  "    return\n"
  "    end\n"
  "  end\n"
  "end)(enable_debug)\n"
  "dbg(' ####### DELETE #######')\n"
  "--delete all the messages right now mister!\n"
  "local msg\n"
  "while true do\n"
  "  msg = redis.call('LPOP', messages)\n"
  "  if msg then\n"
  "    redis.call('DEL', key_msg:format(msg))\n"
  "  else\n"
  "    break\n"
  "  end\n"
  "end\n"
  "\n"
  "local del_msg=\"delete:\" .. id\n"
  "for k,channel_key in pairs(redis.call('SMEMBERS', subscribers)) do\n"
  "  redis.call('PUBLISH', channel_key, del_msg)\n"
  "end\n"
  "redis.call('PUBLISH', pubsub, \"delete\")\n"
  "\n"
  "return redis.call('DEL', key_channel, messages, subscribers)\n",

  //get_message
  "--input:  keys: [], values: [channel_id, msg_time, msg_tag, no_msgid_order, create_channel_ttl, subscriber_channel]\n"
  "--output: result_code, msg_time, msg_tag, message, content_type,  channel_subscriber_count\n"
  "-- no_msgid_order: 'FILO' for oldest message, 'FIFO' for most recent\n"
  "-- create_channel_ttl - make new channel if it's absent, with ttl set to this. 0 to disable.\n"
  "-- result_code can be: 200 - ok, 404 - not found, 410 - gone, 418 - not yet available\n"
  "local id, time, tag, subscribe_if_current = ARGV[1], tonumber(ARGV[2]), tonumber(ARGV[3])\n"
  "local no_msgid_order=ARGV[4]\n"
  "local create_channel_ttl=tonumber(ARGV[5]) or 0\n"
  "local subscriber_channel = ARGV[6]\n"
  "local msg_id\n"
  "if time and time ~= 0 and tag then\n"
  "  msg_id=(\"%s:%s\"):format(time, tag)\n"
  "end\n"
  "\n"
  "local key={\n"
  "  next_message= 'channel:msg:%s:'..id, --not finished yet\n"
  "  message=      'channel:msg:%s:%s', --not done yet\n"
  "  channel=      'channel:'..id,\n"
  "  messages=     'channel:messages:'..id,\n"
  "  pubsub=       'channel:subscribers:'..id\n"
  "}\n"
  "\n"
  "local enable_debug=true\n"
  "local dbg = (function(on)\n"
  "if on then\n"
  "  return function(...)\n"
  "  redis.call('echo', table.concat({...}))\n"
  "end\n"
  "  else\n"
  "    return function(...)\n"
  "    return\n"
  "    end\n"
  "  end\n"
  "end)(enable_debug)\n"
  "\n"
  "dbg(' #######  GET_MESSAGE ######## ')\n"
  "\n"
  "local oldestmsg=function(list_key, old_fmt)\n"
  "  local old, oldkey\n"
  "  local n, del=0,0\n"
  "  while true do\n"
  "    n=n+1\n"
  "    old=redis.call('lindex', list_key, -1)\n"
  "    if old then\n"
  "      oldkey=old_fmt:format(old)\n"
  "      local ex=redis.call('exists', oldkey)\n"
  "      if ex==1 then\n"
  "        return oldkey\n"
  "      else\n"
  "        redis.call('rpop', list_key)\n"
  "        del=del+1\n"
  "      end \n"
  "    else\n"
  "      break\n"
  "    end\n"
  "  end\n"
  "end\n"
  "\n"
  "local tohash=function(arr)\n"
  "  if type(arr)~=\"table\" then\n"
  "    return nil\n"
  "  end\n"
  "  local h = {}\n"
  "  local k=nil\n"
  "  for i, v in ipairs(arr) do\n"
  "    if k == nil then\n"
  "      k=v\n"
  "    else\n"
  "      --dbg(k..\"=\"..v)\n"
  "      h[k]=v; k=nil\n"
  "    end\n"
  "  end\n"
  "  return h\n"
  "end\n"
  "\n"
  "if no_msgid_order ~= 'FIFO' then\n"
  "  no_msgid_order = 'FILO'\n"
  "end\n"
  "\n"
  "local channel = tohash(redis.call('HGETALL', key.channel))\n"
  "if channel == nil then\n"
  "  if create_channel_ttl==0 then\n"
  "    return {404, nil}\n"
  "  end\n"
  "  redis.call('HSET', key.channel, 'time', time)\n"
  "  redis.call('EXPIRE', key.channel, create_channel_ttl)\n"
  "  channel = {time=time}\n"
  "end\n"
  "\n"
  "local subs_count = tonumber(channel.subscribers)\n"
  "\n"
  "if msg_id==nil or #msg_id==0 then\n"
  "  dbg(\"no msg id given, ord=\"..no_msgid_order)\n"
  "  if no_msgid_order == 'FIFO' then --most recent message\n"
  "    dbg(\"get most recent\")\n"
  "    msg_id=channel.current_message\n"
  "  elseif no_msgid_order == 'FILO' then --oldest message\n"
  "    dbg(\"get oldest\")\n"
  "    \n"
  "    msg_id=oldestmsg(key.messages, ('channel:msg:%s:'..id))\n"
  "    \n"
  "    dbg(\"get oldestest\")\n"
  "  end\n"
  "  if msg_id == nil then\n"
  "    return {404, nil}\n"
  "  else\n"
  "    local msg=tohash(redis.call('HGETALL', msg_id))\n"
  "    if subscriber_channel and #subscriber_channel>0 then\n"
  "      --unsubscribe from this channel.\n"
  "      redis.call('SREM',  key.pubsub, subscriber_channel)\n"
  "    end\n"
  "    return {200, tonumber(msg.time) or \"\", tonumber(msg.tag) or \"\", msg.data or \"\", msg.content_type or \"\", subs_count}\n"
  "  end\n"
  "else\n"
  "  key.message=key.message:format(msg_id, id)\n"
  "  if msg_id and channel.current_message == msg_id\n"
  "   or not channel.current_message then\n"
  "\n"
  "    if not channel.current_message then\n"
  "      dbg(\"NEW CHANNEL, MESSAGE NOT READY\")\n"
  "    else\n"
  "      dbg(\"MESSAGE NOT READY\")\n"
  "    end\n"
  "\n"
  "    if subscriber_channel and #subscriber_channel>0 then\n"
  "      --subscribe to this channel.\n"
  "      redis.call('SADD',  key.pubsub, subscriber_channel)\n"
  "    end\n"
  "    return {418, nil}\n"
  "  end\n"
  "\n"
  "  local msg=tohash(redis.call('HGETALL', key.message))\n"
  "\n"
  "  if next(msg) == nil then -- no such message. it might've expired, or maybe it was never there\n"
  "    dbg(\"MESSAGE NOT FOUND\")\n"
  "    --subscribe if necessary\n"
  "    if subscriber_channel and #subscriber_channel>0 then\n"
  "      --subscribe to this channel.\n"
  "      redis.call('SADD',  key.pubsub, subscriber_channel)\n"
  "    end\n"
  "    return {404, nil}\n"
  "  end\n"
  "\n"
  "  local next_msg, next_msgtime, next_msgtag\n"
  "  if not msg.next then --this should have been taken care of by the channel.current_message check\n"
  "    dbg(\"NEXT MESSAGE KEY NOT PRESENT. ERROR, ERROR!\")\n"
  "    return {404, nil}\n"
  "  else\n"
  "    dbg(\"NEXT MESSAGE KEY PRESENT: \" .. msg.next)\n"
  "    key.next_message=key.next_message:format(msg.next)\n"
  "    if redis.call('EXISTS', key.next_message)~=0 then\n"
  "      local ntime, ntag, ndata, ncontenttype=unpack(redis.call('HMGET', key.next_message, 'time', 'tag', 'data', 'content_type'))\n"
  "      return {200, tonumber(ntime) or \"\", tonumber(ntag) or \"\", ndata or \"\", ncontenttype or \"\", subs_count}\n"
  "    else\n"
  "      dbg(\"NEXT MESSAGE NOT FOUND\")\n"
  "      return {404, nil}\n"
  "    end\n"
  "  end\n"
  "end",

  //publish
  "--input:  keys: [], values: [channel_id, time, message, content_type, msg_ttl, subscriber_channel]\n"
  "--output: message_tag, channel_hash {ttl, time_last_seen, subscribers}\n"
  "\n"
  "local id=ARGV[1]\n"
  "local time=tonumber(ARGV[2])\n"
  "local msg={\n"
  "  id=nil,\n"
  "  data= ARGV[3],\n"
  "  content_type=ARGV[4],\n"
  "  ttl= tonumber(ARGV[5]),\n"
  "  time= time,\n"
  "  tag=  0,\n"
  "  last_message=nil,\n"
  "  oldest_message =nil\n"
  "}\n"
  "local enable_debug=true\n"
  "local dbg = (function(on)\n"
  "if on then\n"
  "  return function(...)\n"
  "  redis.call('echo', table.concat({...}))\n"
  "end\n"
  "  else\n"
  "    return function(...)\n"
  "    return\n"
  "    end\n"
  "  end\n"
  "end)(enable_debug)\n"
  "\n"
  "if type(msg.content_type)=='string' and msg.content_type:find(':') then\n"
  "  return {err='Message content-type cannot contain \":\" character.'}\n"
  "end\n"
  "\n"
  "dbg(' #######  PUBLISH   ######## ')\n"
  "\n"
  "-- sets all fields for a hash from a dictionary\n"
  "local hmset = function (key, dict)\n"
  "  if next(dict) == nil then return nil end\n"
  "  local bulk = {}\n"
  "  for k, v in pairs(dict) do\n"
  "    table.insert(bulk, k)\n"
  "    table.insert(bulk, v)\n"
  "  end\n"
  "  return redis.call('HMSET', key, unpack(bulk))\n"
  "end\n"
  "\n"
  "local tohash=function(arr)\n"
  "  if type(arr)~=\"table\" then\n"
  "    return nil\n"
  "  end\n"
  "  local h = {}\n"
  "  local k=nil\n"
  "  for i, v in ipairs(arr) do\n"
  "    if k == nil then\n"
  "      k=v\n"
  "    else\n"
  "      h[k]=v; k=nil\n"
  "    end\n"
  "  end\n"
  "  return h\n"
  "end\n"
  "\n"
  "local key={\n"
  "  time_offset=  'pushmodule:message_time_offset',\n"
  "  last_message= nil,\n"
  "  message=      'channel:msg:%s:'..id, --not finished yet\n"
  "  channel=      'channel:'..id,\n"
  "  messages=     'channel:messages:'..id,\n"
  "  subscribers=  'channel:subscribers:'..id\n"
  "}\n"
  "local channel_pubsub = 'channel:pubsub:'..id\n"
  "\n"
  "local new_channel\n"
  "local channel\n"
  "if redis.call('EXISTS', key.channel) ~= 0 then\n"
  "  channel=tohash(redis.call('HGETALL', key.channel))\n"
  "end\n"
  "\n"
  "if channel~=nil then\n"
  "  dbg(\"channel present\")\n"
  "  if channel.current_message ~= nil then\n"
  "    dbg(\"channel current_message present\")\n"
  "    key.last_message=('channel:msg:%s:%s'):format(channel.current_message, id)\n"
  "  else\n"
  "    dbg(\"channel current_message absent\")\n"
  "    key.last_message=nil\n"
  "  end\n"
  "  new_channel=false\n"
  "else\n"
  "  dbg(\"channel missing\")\n"
  "  channel={}\n"
  "  new_channel=true\n"
  "  key.last_message=nil\n"
  "end\n"
  "\n"
  "--set new message id\n"
  "if key.last_message then\n"
  "  local lastmsg = redis.call('HMGET', key.last_message, 'time', 'tag')\n"
  "  local lasttime, lasttag = tonumber(lastmsg[1]), tonumber(lastmsg[2])\n"
  "  dbg(\"last_time\"..lasttime..\" last_tag\" ..lasttag..\" msg_time\"..msg.time)\n"
  "  if lasttime==msg.time then\n"
  "    msg.tag=lasttag+1\n"
  "  end\n"
  "end\n"
  "msg.id=('%i:%i'):format(msg.time, msg.tag)\n"
  "key.message=key.message:format(msg.id)\n"
  "\n"
  "msg.prev=channel.current_message\n"
  "if key.last_message then\n"
  "  redis.call('HSET', key.last_message, 'next', msg.id)\n"
  "end\n"
  "\n"
  "--update channel\n"
  "redis.call('HSET', key.channel, 'current_message', msg.id)\n"
  "if msg.prev then\n"
  "  redis.call('HSET', key.channel, 'prev_message', msg.prev)\n"
  "end\n"
  "if msg.time then\n"
  "  redis.call('HSET', key.channel, 'time', msg.time)\n"
  "end\n"
  "if not channel.ttl then\n"
  "  channel.ttl=msg.ttl\n"
  "  redis.call('HSET', key.channel, 'ttl', channel.ttl)\n"
  "end\n"
  "\n"
  "--write message\n"
  "hmset(key.message, msg)\n"
  "\n"
  "\n"
  "--check old entries\n"
  "local oldestmsg=function(list_key, old_fmt)\n"
  "  local old, oldkey\n"
  "  local n, del=0,0\n"
  "  while true do\n"
  "    n=n+1\n"
  "    old=redis.call('lindex', list_key, -1)\n"
  "    if old then\n"
  "      oldkey=old_fmt:format(old)\n"
  "      local ex=redis.call('exists', oldkey)\n"
  "      if ex==1 then\n"
  "        return oldkey\n"
  "      else\n"
  "        redis.call('rpop', list_key)\n"
  "        del=del+1\n"
  "      end \n"
  "    else\n"
  "      break\n"
  "    end\n"
  "  end\n"
  "end\n"
  "oldestmsg(key.messages, 'channel:msg:%s:'..id)\n"
  "--update message list\n"
  "redis.call('LPUSH', key.messages, msg.id)\n"
  "\n"
  "--set expiration times for all the things\n"
  "redis.call('EXPIRE', key.message, channel.ttl)\n"
  "redis.call('EXPIRE', key.channel, channel.ttl)\n"
  "redis.call('EXPIRE', key.messages, channel.ttl)\n"
  "--redis.call('EXPIRE', key.subscribers,  channel.ttl)\n"
  "\n"
  "--publish message\n"
  "local msgpacked = cmsgpack.pack({time=msg.time, tag=msg.tag, content_type=msg.content_type, data=msg.data, channel=id})\n"
  "local pubsub_message=('%i:%i:%s:%s'):format(msg.time, msg.tag, msg.content_type, msg.data)\n"
  "\n"
  "for k,channel_key in pairs(redis.call('SMEMBERS', key.subscribers)) do\n"
  "  --not efficient, but useful for a few short-term subscriptions\n"
  "  redis.call('PUBLISH', channel_key, msgpacked)\n"
  "end\n"
  "--clear short-term subscriber list\n"
  "redis.call('DEL', key.subscribers)\n"
  "--now publish to the efficient channel\n"
  "redis.call('PUBLISH', channel_pubsub, pubsub_message)\n"
  "\n"
  "\n"
  "return { msg.tag, {channel.ttl or msg.ttl, channel.time or msg.time, channel.subscribers or 0}, new_channel}",

  //publish_status
  "--input:  keys: [], values: [channel_id, status_code]\n"
  "--output: current_subscribers\n"
  "local enable_debug=true\n"
  "local dbg = (function(on)\n"
  "if on then\n"
  "  return function(...)\n"
  "  redis.call('echo', table.concat({...}))\n"
  "end\n"
  "  else\n"
  "    return function(...)\n"
  "    return\n"
  "    end\n"
  "  end\n"
  "end)(enable_debug)\n"
  "dbg(' ####### PUBLISH STATUS ####### ')\n"
  "local id=ARGV[1]\n"
  "local code=tonumber(ARGV[2])\n"
  "if code==nil then\n"
  "  return {err=\"non-numeric status code given, bailing!\"}\n"
  "end\n"
  "\n"
  "local pubmsg = \"status:\"..code\n"
  "local subs_key = 'channel:subscribers:'..id\n"
  "local chan_key = 'channel:'..id\n"
  "\n"
  "for k,channel_key in pairs(redis.call('SMEMBERS', subs_key)) do\n"
  "  --not efficient, but useful for a few short-term subscriptions\n"
  "  redis.call('PUBLISH', channel_key, pubmsg)\n"
  "end\n"
  "--clear short-term subscriber list\n"
  "redis.call('DEL', subs_key)\n"
  "--now publish to the efficient channel\n"
  "redis.call('PUBLISH', channel_pubsub, pubmsg)\n"
  "return redis.call('HGET', chan_key, 'subscribers') or 0",

  //subscribe
  "--purposely left blank\n"
};

