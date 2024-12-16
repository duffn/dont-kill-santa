local GameStateManager = require("lib/GameStateManager")
local Assets = require("assets")

local Instructions = {}

function Instructions:new()
  local obj = {
    title = "Instructions",
    play = "Play",
    instructions_start_y = 250,
    hand_cursor = love.mouse.getSystemCursor("hand"),
  }
  setmetatable(obj, self)
  self.__index = self
  return obj
end

function Instructions:enter()
  love.mouse.setCursor()

  self.font = Assets.fonts.title
  self.menu_font = Assets.fonts.menu
  self.menu_font:setFilter("nearest", "nearest")
  self.santa_walk = Assets.sprites.santa_walk
  self.background_color = Assets.colors.background
  self.background = Assets.sprites.background
  self.instructions_font = love.graphics.newFont("assets/fonts/coolvetica.ttf", 24)
  self.santa_walk = Assets.sprites.santa_walk
  self.santa_walk_width = self.santa_walk:getWidth() / SANTA_WALK_FRAMES

  -- Calculate the bounding box for "Play" text
  self.play_width = self.menu_font:getWidth(self.play)
  self.play_height = self.menu_font:getHeight()
  self.play_x = (love.graphics.getWidth() - self.play_width) / 2.0
  self.play_y = (love.graphics.getHeight() - self.play_height) / 2.0 + 175
end

local function pointInRect(px, py, rx, ry, rw, rh)
  return px >= rx and px <= rx + rw and py >= ry and py <= ry + rh
end

function Instructions:quit()
  self.instructions_font:release()
end

function Instructions:update(dt) end

function Instructions:mousepressed(x, y, button)
  if button == 1 then
    if pointInRect(x, y, self.play_x, self.play_y, self.play_width, self.play_height) then
      local Game = require("states/game")
      GameStateManager:setState(Game:new())
    end
  end
end

function Instructions:draw()
  love.graphics.setColor(table.unpack(Assets.colors.white))
  love.graphics.draw(self.background, 0, 0)

  -- Draw the title
  love.graphics.setFont(self.font)
  love.graphics.print(self.title, (love.graphics.getWidth() - self.font:getWidth(self.title)) / 2.0, 10)

  -- Draw the instructions
  love.graphics.setColor(table.unpack(Assets.colors.background))
  love.graphics.rectangle("fill", 0, self.instructions_start_y - 25, love.graphics.getWidth(), 200)

  love.graphics.setColor(table.unpack(Assets.colors.white))
  love.graphics.setFont(self.instructions_font)
  self.instructions_font:setFilter("nearest", "nearest")
  love.graphics.print(
    "Hold down H or O or hold down on a touchscreen to make santa run. HO, HO, HO!",
    10,
    self.instructions_start_y
  )
  love.graphics.print(
    "Release the key or remove your finger from the touchscreen to stop him.",
    10,
    self.instructions_start_y + 40
  )
  love.graphics.print(
    "Stop as close as possible to the candy cane, but don't touch it or you know what happens!",
    10,
    self.instructions_start_y + 80
  )
  love.graphics.print(
    "The closer you get to the candy cane, the better, so try for your lowest score!",
    10,
    self.instructions_start_y + 120
  )

  -- Draw santa
  love.graphics.draw(
    self.santa_walk,
    love.graphics.newQuad(
      self.santa_walk_width * 4,
      0,
      self.santa_walk_width,
      self.santa_walk:getHeight(),
      self.santa_walk:getDimensions()
    ),
    0,
    love.graphics.getHeight() - self.santa_walk:getHeight() - 10,
    0,
    1
  )

  -- Draw play button
  love.graphics.setFont(self.menu_font)
  love.graphics.setColor(table.unpack(Assets.colors.white))
  love.graphics.print(self.play, self.play_x, self.play_y)
end

return Instructions
