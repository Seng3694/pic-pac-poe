local function read_file(path)
    local file = io.open(path, "r")
    if file == nil then
        return nil
    end
    local contents = file:read("a")
    file:close()
    return contents
end

local function write_file(path, contens)
    local file = io.open(path, "w")
    if file == nil then
        return nil
    end
    file:write(contens)
    file:close()
end

local function get_file_name(path)
    return path:match("([^%/]+)%..+$")
end

function Export(textures, outputFile)
    table.sort(textures, function(a, b)
        return a.name:lower() < b.name:lower()
    end)

    local lower = get_file_name(outputFile):lower()
    local upper = lower:upper()

    local enumDefinitions = {}
    local rectDefinitions = {}
    for _, v in ipairs(textures) do
        local enumDefinition = upper .. "_ID_" .. get_file_name(v.name):upper()
        local rectDefinition = "  { "
            .. tostring(v.left) .. ", "
            .. tostring(v.top) .. ", "
            .. tostring(v.width) .. ", "
            .. tostring(v.height) .. " },"
        table.insert(enumDefinitions, "  " .. enumDefinition)
        table.insert(rectDefinitions, rectDefinition)
    end

    write_file(outputFile, read_file("example/header_template.h.txt")
        :gsub("<UPPER_CASE_NAME>", upper)
        :gsub("<LOWER_CASE_NAME>", lower)
        :gsub("<ENUM_DEFINITIONS>", table.concat(enumDefinitions, ",\n"))
        :gsub("<RECT_DEFINITIONS>", table.concat(rectDefinitions, "\n")))
end
