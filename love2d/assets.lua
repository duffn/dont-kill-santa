local Assets = {}

function Assets:load()
  -- Fonts
  self.fonts = {
    title = love.graphics.newFont("assets/fonts/hello-santa.ttf", 96),
    menu = love.graphics.newFont("assets/fonts/coolvetica.ttf", 36),
  }

  -- Sprites
  self.sprites = {
    santa_dead = love.graphics.newImage("assets/sprites/santa-dead.png"),
    santa_walk = love.graphics.newImage("assets/sprites/santa-walk.png"),
    candy_cane = love.graphics.newImage("assets/sprites/candycane.png"),
    background = love.graphics.newImage("assets/backgrounds/background.png"),
    ground = love.graphics.newImage("assets/backgrounds/ground.png"),
  }

  -- Colors
  self.colors = {
    background = { love.math.colorFromBytes(47, 79, 146) },
    white = { 1, 1, 1 },
  }
end

return Assets
