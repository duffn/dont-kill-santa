local GameStateManager = require("lib/GameStateManager")
local Assets = require("assets")

local Title = require("states/title")

require("globals")

function love.load()
  love.window.setMode(1280, 720)

  Assets:load()

  GameStateManager:setState(Title:new())
end

function love.update(dt)
  GameStateManager:update(dt)
end

function love.draw()
  GameStateManager:draw()
end

function love.mousepressed(x, y, button)
  GameStateManager:mousepressed(x, y, button)
end
