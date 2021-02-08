player.setNextDestination = function(map)
    if player.position.x < map.x + 0.5 * map.width then
        player.destination.x = map.x + map.width
    else
        player.destination.x = map.x
    end
end
player.aim = function(enemy)
    player.aimingDirection.x = player.position.x - enemy:getPosition().x
    player.aimingDirection.y = enemy:getPosition().y - player.position.y
end