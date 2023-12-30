local function rect_fits(width1, height1, width2, height2)
    if width1 == width2 and height1 == height2 then
        return 1
    elseif width1 <= width2 and height1 <= height2 then
        return 2
    else
        return 0
    end
end

local function node_insert(node, rect)
    if node.left ~= nil then
        if node_insert(node.left, rect) then
            return true
        else
            return node_insert(node.right, rect)
        end
    end
    if node.filled then
        return false
    end

    local result = rect_fits(rect.width, rect.height, node.width, node.height)
    if result == 1 then -- perfect fit
        node.filled = true
        rect.left = node.x
        rect.top = node.y
        return true
    elseif result == 0 then -- no fit
        return false
    end

    node.left = {
        x = 0,
        y = 0,
        width = 0,
        height = 0,
        filled = false,
        left = nil,
        right = nil
    }
    node.right = {
        x = 0,
        y = 0,
        width = 0,
        height = 0,
        filled = false,
        left = nil,
        right = nil
    }
    -- if it fits but there is still space left
    if node.width - rect.width > node.height - rect.height then
        node.left.x = node.x
        node.left.y = node.y
        node.left.width = rect.width
        node.left.height = node.height

        node.right.x = node.x + rect.width
        node.right.y = node.y
        node.right.width = (node.x + node.width) - node.right.x
        node.right.height = node.height
    else
        node.left.x = node.x
        node.left.y = node.y
        node.left.width = node.width
        node.left.height = rect.height

        node.right.x = node.x
        node.right.y = node.y + rect.height
        node.right.width = node.width
        node.right.height = (node.y + node.height) - node.right.y
    end

    return node_insert(node.left, rect)
end

local function node_reset(node)
    if node.left ~= nil then
        node_reset(node.left)
        node_reset(node.right)
        node.left = nil
        node.right = nil
    end
    node.filled = false
    node.width = 0
    node.height = 0
    node.x = 0
    node.y = 0
end

function Pack(rectangles, width)
    -- sort by area in descending order
    table.sort(
        rectangles,
        function(a, b)
            return (b.width * b.height) < (a.width * a.height)
        end)

    local totalArea = 0
    for _, v in ipairs(rectangles) do
        totalArea = totalArea + (v.width * v.height)
    end

    local height = 0
    local fixedLength = false
    if width ~= 0 then
        height = totalArea // width
        fixedLength = true
    else
        width = math.floor(math.sqrt(totalArea))
        height = width
    end
    local root = {
        x = 0,
        y = 0,
        width = width,
        height = height,
        filled = false,
        left = nil,
        right = nil
    }
    local result = false
    while not result do
        for _, r1 in ipairs(rectangles) do
            result = node_insert(root, r1)
            if not result then
                node_reset(root)
                if not fixedLength then
                    width = width + 8
                end
                height = height + 8
                root.width = width
                root.height = height
                for _, r2 in ipairs(rectangles) do
                    r2.left = 0
                    r2.top = 0
                end

                break
            end
        end
    end

    return rectangles, width, height
end
