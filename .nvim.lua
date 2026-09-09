local dap = require("dap")

dap.adapters.gdb = {
  id = "gdb",
  type = "executable",
  command = "gdb",
  args = { "--quiet", "--interpreter=dap" },
}

-- Repo root = directory containing this .nvim.lua.
-- More robust than vim.fn.getcwd() / "${workspaceFolder}".
local root = vim.fn.fnamemodify(debug.getinfo(1, "S").source:sub(2), ":p:h")

dap.configurations.c = {
  {
    name = "test json",
    type = "gdb",
    request = "launch",
    program = function()
      -- Build first. NOTE: build-test.sh also *runs* the tests by default,
      -- which looks like "the program executes" outside the debugger.
      -- Pass --build-only so the only run happens under gdb.
      local out = vim.fn.system({ "sh", root .. "/tests/build-test.sh", "--build-only" })
      if vim.v.shell_error ~= 0 then
        vim.notify("Build failed:\n" .. out, vim.log.levels.ERROR)
        error("Build failed, aborting debug session")
      end
      -- IMPORTANT: must return an absolute path. nvim-dap does NOT expand
      -- "${workspaceFolder}" inside a function return value, so returning the
      -- literal "${workspaceFolder}/tests/json_test" leaves gdb with no
      -- symbols loaded and every breakpoint stays pending with:
      --   "No source file named ...".
      return root .. "/tests/json_test"
    end,
    cwd = root,
    -- gdb DAP option (was: stopOnEntry, which gdb ignores).
    stopAtBeginningOfMainSubprogram = true,
    args = { "-v" },
  },
}
