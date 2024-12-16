local GameStateManager = require("lib/GameStateManager")
local Assets = require("assets")

local Game = {}

function Game:new()
  local obj = {
    target_fps = 60,
    frame_multiplier = 55,
    acceleration = 200,
    max_frame_speed = 37,
    deceleration = 100,
    restart_text = "Restart",
    quit_text = "Quit",
    game_over_text = "You Killed Santa",
  }
  setmetatable(obj, self)
  self.__index = self
  return obj
end

local function pointInRect(px, py, rx, ry, rw, rh)
  return px >= rx and px <= rx + rw and py >= ry and py <= ry + rh
end

function Game:enter()
  -- Load assets (adjust paths as needed)
  self.background = Assets.sprites.background
  self.ground = Assets.sprites.ground
  self.font = Assets.fonts.title
  self.menu_font = Assets.fonts.menu
  self.santa_dead = Assets.sprites.santa_dead
  self.santa_walk = Assets.sprites.santa_walk
  self.candy_cane = Assets.sprites.candy_cane

  self.ground_y = love.graphics.getHeight() - self.ground:getHeight()

  -- Santa frame setup
  self.santa_frame_width = self.santa_walk:getWidth() / SANTA_WALK_FRAMES
  self.santa_frame_height = self.santa_walk:getHeight()
  self.santa_frame_x = 0
  self.santa_frame_y = 0

  self.current_santa = self.santa_walk

  -- Position Santa on the ground
  self.santa_position = { x = 0, y = self.ground_y - self.santa_frame_height + 20 }

  -- Candy cane starts off-screen
  self.candy_cane_position = {
    x = love.graphics.getWidth() + math.random(100, 5000),
    y = self.ground_y - self.candy_cane:getHeight() + 10,
  }

  -- Measure text sizes for menu options
  love.graphics.setFont(self.menu_font)
  local restart_w = self.menu_font:getWidth(self.restart_text)
  local restart_h = self.menu_font:getHeight()
  self.restart_text_size = { x = restart_w, y = restart_h }
  self.restart_text_position = {
    x = (love.graphics.getWidth() - restart_w) / 2,
    y = (love.graphics.getHeight() - restart_h) / 2 + 100,
  }

  local quit_w = self.menu_font:getWidth(self.quit_text)
  local quit_h = self.menu_font:getHeight()
  self.quit_text_size = { x = quit_w, y = quit_h }
  self.quit_text_position = {
    x = (love.graphics.getWidth() - quit_w) / 2,
    y = (love.graphics.getHeight() - quit_h) / 2 + 150,
  }

  love.graphics.setFont(self.font)
  local go_w = self.font:getWidth(self.game_over_text)
  local go_h = self.font:getHeight()
  self.game_over_text_size = { x = go_w, y = go_h }
  self.game_over_text_position = {
    x = (love.graphics.getWidth() - go_w) / 2,
    y = (love.graphics.getHeight() - go_h) / 2,
  }

  -- Initialize variables
  self.scrolling_back = 0
  self.frames_speed = 0
  self.frames_counter = 0
  self.current_frame = 0

  self.is_dead = false
  self.is_animating_death = false
  self.game_started = false
  self.game_ended = false
  self.final_difference = 0
  self.current_difference = 0
end

function Game:update(dt)
  -- Update background scrolling
  if not self.is_dead and self.frames_speed > 0 then
    self.scrolling_back = self.scrolling_back - self.frames_speed * dt * self.frame_multiplier
  end

  if self.scrolling_back <= -self.background:getWidth() then
    self.scrolling_back = 0
  end

  -- Handle input for movement
  local hDown = love.keyboard.isDown("h")
  local oDown = love.keyboard.isDown("o")
  local isHolding = hDown or oDown
  if not self.is_dead and not self.game_ended and isHolding then
    self.frames_speed = self.frames_speed + self.acceleration * dt
    if self.frames_speed > self.max_frame_speed then
      self.frames_speed = self.max_frame_speed
    end
  elseif not self.is_animating_death then
    -- Decelerate if no input
    self.frames_speed = self.frames_speed - self.deceleration * dt
    if self.frames_speed < 0 then
      self.frames_speed = 0
    end
  end

  if not self.game_started and self.frames_speed > 0 then
    self.game_started = true
  end

  -- Update animations if Santa moves or is animating death
  if self.frames_speed > 0 or self.is_animating_death then
    self.frames_counter = self.frames_counter + self.target_fps * dt * 3

    local frameDuration = (self.target_fps / (self.frames_speed > 0 and self.frames_speed or 1))
    if self.frames_counter >= frameDuration or self.is_animating_death then
      self.frames_counter = 0
      self.current_frame = self.current_frame + 1

      if self.is_dead then
        -- Death animation
        if self.current_frame >= SANTA_DEAD_FRAMES then
          self.current_frame = SANTA_DEAD_FRAMES - 1
          self.is_animating_death = false
          if not self.game_ended then
            self.game_ended = true
            self.final_difference = self.current_difference
          end
        end
      else
        -- Walking animation
        if self.current_frame >= SANTA_WALK_FRAMES then
          self.current_frame = 0
        end
      end

      if self.is_dead then
        -- Update frame width and height for dead sprite
        local frame_w = self.santa_dead:getWidth() / SANTA_DEAD_FRAMES
        local frame_h = self.santa_dead:getHeight()
        self.santa_frame_width = frame_w
        self.santa_frame_height = frame_h
        self.santa_frame_x = self.current_frame * frame_w
        self.santa_frame_y = 0
      else
        -- Update frame for walking
        self.santa_frame_x = self.current_frame * self.santa_frame_width
        self.santa_frame_y = 0
      end
    end

    if not self.is_dead then
      self.candy_cane_position.x = self.candy_cane_position.x - self.frames_speed * dt * self.frame_multiplier
    end
  end

  local santa_front_x = self.santa_position.x + (self.santa_frame_width / 2) + (self.santa_frame_width * 0.08)
  local candy_cane_front_x = self.candy_cane_position.x + (self.candy_cane:getWidth() * 0.28)
  self.current_difference = candy_cane_front_x - santa_front_x

  -- Check collision or passing
  if not self.is_dead and santa_front_x >= candy_cane_front_x then
    self.is_dead = true
    self.is_animating_death = true
    self.current_santa = self.santa_dead
    self.current_frame = 0
    self.santa_frame_width = self.santa_dead:getWidth() / SANTA_DEAD_FRAMES
    self.santa_frame_height = self.santa_dead:getHeight()
    self.santa_frame_x = 0
    self.santa_frame_y = 0
  end

  -- End game if Santa stops after starting
  if self.game_started and self.frames_speed == 0 and not self.game_ended and not self.is_dead then
    self.game_ended = true
    self.final_difference = self.current_difference
  end
end

function Game:mousepressed(x, y, button)
  if button == 1 and self.game_ended then
    -- Check if click is on restart button
    if
      pointInRect(
        x,
        y,
        self.restart_text_position.x,
        self.restart_text_position.y,
        self.restart_text_size.x,
        self.restart_text_size.y
      )
    then
      GameStateManager:setState(Game:new())
    end

    -- Check if click is on quit button
    if
      pointInRect(
        x,
        y,
        self.quit_text_position.x,
        self.quit_text_position.y,
        self.quit_text_size.x,
        self.quit_text_size.y
      )
    then
      local Title = require("states/title")
      GameStateManager:setState(Title:new())
    end
  end
end

function Game:draw()
  love.graphics.clear(1, 1, 1) -- RAYWHITE is basically white

  -- Draw scrolling background
  love.graphics.draw(self.background, self.scrolling_back, 0)
  love.graphics.draw(self.background, self.background:getWidth() + self.scrolling_back, 0)

  -- Draw candy cane
  love.graphics.draw(self.candy_cane, self.candy_cane_position.x, self.candy_cane_position.y)

  -- Draw ground
  for x = 0, love.graphics.getWidth(), self.ground:getWidth() do
    love.graphics.draw(self.ground, x, self.ground_y)
  end

  -- Draw Santa
  -- Create a quad for current frame
  local santa_quad = love.graphics.newQuad(
    self.santa_frame_x,
    self.santa_frame_y,
    self.santa_frame_width,
    self.santa_frame_height,
    self.current_santa:getWidth(),
    self.current_santa:getHeight()
  )
  love.graphics.draw(self.current_santa, santa_quad, self.santa_position.x, self.santa_position.y)

  if self.game_ended then
    love.graphics.setFont(self.font)
    if self.final_difference < 0 then
      love.graphics.print(self.game_over_text, self.game_over_text_position.x, self.game_over_text_position.y)
    else
      local game_over_score_text = string.format("Distance: %.2f", self.final_difference)
      local score_w = self.font:getWidth(game_over_score_text)
      local score_h = self.font:getHeight()
      local score_x = (love.graphics.getWidth() - score_w) / 2
      local score_y = (love.graphics.getHeight() - score_h) / 2
      love.graphics.print(game_over_score_text, score_x, score_y)
    end

    love.graphics.setFont(self.menu_font)
    love.graphics.print(self.restart_text, self.restart_text_position.x, self.restart_text_position.y)
    love.graphics.print(self.quit_text, self.quit_text_position.x, self.quit_text_position.y)
  end
end

return Game
