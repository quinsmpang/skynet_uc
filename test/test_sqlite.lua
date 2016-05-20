local skynet = require "skynet"
local sqlite3 = require("lsqlite3")


local db = sqlite3.open("sqlite3.db")

skynet.start(function()

	print("Before start")

	db:exec[[
	  CREATE TABLE test (id INTEGER PRIMARY KEY, content);

	  INSERT INTO test VALUES (NULL, 'Hello World');
	  INSERT INTO test VALUES (NULL, 'Hello Lua');
	  INSERT INTO test VALUES (NULL, 'Hello Sqlite3')
	]]

	for row in db:nrows("SELECT * FROM test") do
	  print(row.id, row.content)
	end

	db:close()
	
	print("End satrt")

end)
