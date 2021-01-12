player.setNextDestination = function(map)
    if player.position.x < map.x + 0.5 * map.width then
        player.destination.x = map.x + 0.9 * map.width
    else
        player.destination.x = map.x + 0.1 * map.width
    end
end
