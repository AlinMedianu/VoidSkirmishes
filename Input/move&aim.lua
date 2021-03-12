-- "player" is a table containing a "position", "destination", "movementSpeed"
-- "facingDirection", "aimingDirection" and "turningSpeed". The two speeds 
-- are real numbers, but the rest are of type Vector2, which ecah have
-- an x and a y that are real numbers as well. Where the position and destination
-- are normal screen coordinate positions, the aiming and facing directions are normalized vectors.
-- These 6 variables can't be changed usually, but "destination" can be changed 
-- in the function "setNextDestination", while "aimingDirection" can be changed
-- in the function "aim"

-- The function "setNextDestination" takes in a Rect called map,
-- a rectangle has an x and a y that point to the top left of the rectangle,
-- a rectangle also has a width and a height.
-- The function "setNextDestination" is called wheneve the player's destination 
-- is the same as its position. As the name suggest this function is meant to 
-- change the destination of the player. As soon as this function changes the 
-- destination to not be the same as the position, the player will proceed to
-- move at a constant speed ("movementSpeed") from its current position to 
-- its destination. When the player finally reaches the destination, 
-- the function will be called again.
-- The map cannot be changed. The player can't exit the map, neither.
-- The map only has positive coordinates, the x coordinate grows from
-- the left to the right of the screen and the y coordinate grows from
-- the top to the bottom of the screen. Given that, the direction vectors
-- will have a positive x towards the right of the screen, a negative x towards 
-- the left and 0 looking up or down. The direction vectors will also have a positive
-- y towards the bottom of the screen, negative y towards the top and 0 towards left or right
player.setNextDestination = function(map)
    if player.position.x < map.x + 0.5 * map.width then
        player.destination.x = map.x + map.width
    else
        player.destination.x = map.x
    end
end

-- The function "aim" takes in a Character called enemy,
-- a character only has a function called getPosition which is a Vector2,
-- The function "aim" is called wheneve the player's aiming direction 
-- is the same as its facing direction. This function is meant to 
-- change the aiming direction of the player. As soon as this function changes the 
-- aiming direction to not be the same as the facing direction, the player will proceed to
-- rotate at a constant speed ("turningSpeed") from its current facing direction to 
-- its aiming direction. When the player is finally facing the same way as the aiming direction, 
-- the function will be called again, and if the function changes the aiming direction to not be
-- the same as the facing direction, the player will shoot a laser towards its facing direction
-- that could hit and damage the enemy. The laser has a 1 second cooldown, so it can't be spammed
-- continuously. Also, the player doesn't collide with the enemy, it will go right through it,
-- do don't need to worry about that.
player.aim = function(enemy)
    player.aimingDirection.x = player.position.x - enemy:getPosition().x
    player.aimingDirection.y = enemy:getPosition().y - player.position.y
end