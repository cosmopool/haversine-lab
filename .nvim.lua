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
      local out = vim.fn.system({ "sh", root .. "/tests/build-test.sh" })
      if vim.v.shell_error ~= 0 then
        vim.notify("Build failed:\n" .. out, vim.log.levels.ERROR)
        error("Build failed, aborting debug session")
      end
      return root .. "/tests/json_test"
    end,
    cwd = root,
    stopAtBeginningOfMainSubprogram = true,
    args = { "-v" },
  },
}
