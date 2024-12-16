local GameStateManager = require("lib/GameStateManager")
local Assets = require("assets")

local Title = {}

function Title:new()
  local obj = {
    start = "Start",
    instructions = "Instructions",
    title = "Don't Kill Santa",
    hand_cursor = love.mouse.getSystemCursor("hand"),
  }
  setmetatable(obj, self)
  self.__index = self
  return obj
end

local function pointInRect(px, py, rx, ry, rw, rh)
  return px >= rx and px <= rx + rw and py >= ry and py <= ry + rh
end

function Title:enter()
  print("Title:enter")
  love.mouse.setCursor()

  self.font = Assets.fonts.title
  self.menu_font = Assets.fonts.menu
  self.menu_font:setFilter("nearest", "nearest")
  self.santa_dead = Assets.sprites.santa_dead
  self.santa_dead_width = self.santa_dead:getWidth() / SANTA_DEAD_FRAMES
  self.background_color = Assets.colors.background

  -- Calculate the bounding box for "Start" text
  self.start_width = self.menu_font:getWidth(self.start)
  self.start_height = self.menu_font:getHeight()
  self.start_x = (love.graphics.getWidth() - self.start_width) / 2.0
  self.start_y = (love.graphics.getHeight() - self.start_height) / 2.0 + 100

  -- Calculate the bounding box for "Instructions" text
  self.instructions_width = self.menu_font:getWidth(self.instructions)
  self.instructions_height = self.menu_font:getHeight()
  self.instructions_x = (love.graphics.getWidth() - self.instructions_width) / 2.0
  self.instructions_y = (love.graphics.getHeight() - self.instructions_height) / 2.0 + 150
end

function Title:update(dt) end

function Title:mousepressed(x, y, button)
  if button == 1 then
    if pointInRect(x, y, self.start_x, self.start_y, self.start_width, self.start_height) then
      local Game = require("states/game")
      GameStateManager:setState(Game:new())
    end

    if
      pointInRect(x, y, self.instructions_x, self.instructions_y, self.instructions_width, self.instructions_height)
    then
      local Instructions = require("states/instructions")
      GameStateManager:setState(Instructions:new())
    end
  end
end

function Title:draw()
  love.graphics.setBackgroundColor(table.unpack(self.background_color))
  love.graphics.setColor(table.unpack(Assets.colors.white))

  -- Title
  love.graphics.setFont(self.font)
  love.graphics.print(
    self.title,
    (love.graphics.getWidth() - self.font:getWidth(self.title)) / 2.0,
    (love.graphics.getHeight() - self.font:getHeight()) / 2.0 - 150
  )

  -- santa_dead Sprite
  love.graphics.draw(
    self.santa_dead,
    love.graphics.newQuad(
      self.santa_dead_width * 16,
      0,
      self.santa_dead_width,
      self.santa_dead:getHeight(),
      self.santa_dead:getDimensions()
    ),
    (love.graphics.getWidth() - self.santa_dead_width) / 2.0 - 50,
    (love.graphics.getHeight() - self.santa_dead:getHeight()) / 2.0 - 80,
    0,
    1
  )

  -- Start Button
  love.graphics.setFont(self.menu_font)
  love.graphics.print(self.start, self.start_x, self.start_y)

  -- Instructions Button
  love.graphics.print(self.instructions, self.instructions_x, self.instructions_y)
end

return Title
