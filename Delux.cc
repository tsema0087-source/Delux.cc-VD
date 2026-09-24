--[[
    delux.cc | Меню + Stretch + ESP + ESP2.0 + Combat + Watermark + Fullbright + FOV + TP + Fog + Save + Noclip + KeybindList + Notifications + Lightborn
    ШРИФТ: Gotham (единый для всего меню)
]]
local Players = game:GetService("Players")
local RunService = game:GetService("RunService")
local TweenService = game:GetService("TweenService")
local Debris = game:GetService("Debris")
local CoreGui = game:GetService("CoreGui")
local Stats = game:GetService("Stats")
local UserInputService = game:GetService("UserInputService")
local Lighting = game:GetService("Lighting")
local Camera = workspace.CurrentCamera
local player = Players.LocalPlayer

-----------------------------------------------------------
-- // ЕДИНЫЙ ШРИФТ
-----------------------------------------------------------
local FONT = Enum.Font.Gotham
local FONT_BOLD = Enum.Font.GothamBold
local FONT_MEDIUM = Enum.Font.GothamMedium

-----------------------------------------------------------
-- // СИСТЕМА СОХРАНЕНИЯ
-----------------------------------------------------------
local SaveFolder = "delux_cc"
local SaveFile = SaveFolder .. "/config.json"

local hasFS = (typeof(writefile) == "function") and (typeof(readfile) == "function") and (typeof(isfile) == "function")
if hasFS then
    pcall(function()
        if not isfolder(SaveFolder) then
            makefolder(SaveFolder)
        end
    end)
end

local Defaults = {
    ESPEnabled = false,
    ESP2Enabled = false,
    GenESPEnabled = false,
    PalletESPEnabled = false,
    FullbrightEnabled = false,
    FullbrightValue = 5,
    FOVValue = 70,
    WalkSpeedValue = 16,
    JumpEnabled = false,
    JumpPowerValue = 50,
    BlueFogEnabled = false,
    NoclipEnabled = false,
    LightbornEnabled = false,
    TPKillerKeybind = "NONE",
    WalkSpeedKeybind = "NONE",
    NoclipKeybind = "NONE",
    WalkSpeedActive = false,
    KeybindListEnabled = false,
    KeybindListPosX = 20,
    KeybindListPosY = 100,
}

local function findKeyCodeByName(name)
    if not name or typeof(name) ~= "string" then return nil end
    if name == "NONE" or name == "" then return nil end
    local ok, kc = pcall(function() return Enum.KeyCode[name] end)
    if ok and kc then return kc end
    for _, code in ipairs(Enum.KeyCode:GetEnumItems()) do
        if code.Name:lower() == name:lower() then
            return code
        end
    end
    return nil
end

local function loadConfig()
    if not hasFS then return end
    local ok, content = pcall(readfile, SaveFile)
    if not ok or not content or content == "" then return end
    local ok2, data = pcall(function() return game:GetService("HttpService"):JSONDecode(content) end)
    if not ok2 or typeof(data) ~= "table" then return end

    for k, v in pairs(data) do
        if k == "TPKillerKeybind" or k == "WalkSpeedKeybind" or k == "NoclipKeybind" then
            if typeof(v) == "string" and v ~= "NONE" then
                local kc = findKeyCodeByName(v)
                if kc then getgenv()[k] = kc end
            else
                getgenv()[k] = nil
            end
        else
            getgenv()[k] = v
        end
    end
end

local function saveConfig()
    if not hasFS then return end
    local data = {}
    for k, v in pairs(Defaults) do
        local val = getgenv()[k]
        if val == nil then
            data[k] = "NONE"
        elseif typeof(val) == "EnumItem" then
            data[k] = val.Name
        else
            data[k] = val
        end
    end
    local ok, encoded = pcall(function()
        return game:GetService("HttpService"):JSONEncode(data)
    end)
    if ok and encoded then
        pcall(writefile, SaveFile, encoded)
    end
end

for k, v in pairs(Defaults) do
    if getgenv()[k] == nil then
        if v == "NONE" then
            getgenv()[k] = nil
        else
            getgenv()[k] = v
        end
    end
end
loadConfig()

-----------------------------------------------------------
-- // РАСТЯГИВАНИЕ 4:3
-----------------------------------------------------------
if getgenv().StretchActive == nil then
    getgenv().StretchConnection = RunService.RenderStepped:Connect(function()
        Camera.CFrame = Camera.CFrame * CFrame.new(0, 0, 0, 1, 0, 0, 0, 0.67, 0, 0, 0, 1)
    end)
end
getgenv().StretchActive = true

-----------------------------------------------------------
-- // Цвета
-----------------------------------------------------------
local Colors = {
    Background = Color3.fromRGB(20, 20, 20),
    Sidebar = Color3.fromRGB(15, 15, 15),
    Element = Color3.fromRGB(35, 35, 35),
    Text = Color3.fromRGB(240, 240, 240),
    TextDim = Color3.fromRGB(150, 150, 150),
    ToggleOff = Color3.fromRGB(60, 60, 60),
    ToggleOn = Color3.fromRGB(255, 255, 255),
    Accent = Color3.fromRGB(0, 150, 255),
    Accent2 = Color3.fromRGB(150, 80, 255),
    Button = Color3.fromRGB(45, 45, 55),
    ButtonHover = Color3.fromRGB(60, 60, 75),
    GenGreen = Color3.fromRGB(0, 255, 80),
    PalletOrange = Color3.fromRGB(255, 130, 20),
    KillerRed = Color3.fromRGB(255, 40, 40),
    SurvivorGreen = Color3.fromRGB(0, 255, 80),
    KeybindBG = Color3.fromRGB(50, 50, 60),
    KeybindListening = Color3.fromRGB(255, 150, 0),
}

-----------------------------------------------------------
-- // ScreenGui
-----------------------------------------------------------
local ScreenGui = Instance.new("ScreenGui")
ScreenGui.Name = "DeluxCC_Menu"
ScreenGui.ResetOnSpawn = false
ScreenGui.IgnoreGuiInset = true
ScreenGui.DisplayOrder = 2147483647
ScreenGui.ZIndexBehavior = Enum.ZIndexBehavior.Sibling

local ok, _ = pcall(function() ScreenGui.Parent = CoreGui end)
if not ok then ScreenGui.Parent = player:WaitForChild("PlayerGui") end

-----------------------------------------------------------
-- // NOTIFICATION SYSTEM
-----------------------------------------------------------
local NotifContainer = Instance.new("Frame")
NotifContainer.Name = "Notifications"
NotifContainer.Size = UDim2.new(0, 280, 1, -120)
NotifContainer.Position = UDim2.new(0, 20, 0, 60)
NotifContainer.BackgroundTransparency = 1
NotifContainer.ZIndex = 4500
NotifContainer.Parent = ScreenGui

local notifLayout = Instance.new("UIListLayout")
notifLayout.SortOrder = Enum.SortOrder.LayoutOrder
notifLayout.VerticalAlignment = Enum.VerticalAlignment.Bottom
notifLayout.Padding = UDim.new(0, 6)
notifLayout.Parent = NotifContainer

local activeNotifs = 0

local function showNotification(title, subtitle)
    if activeNotifs >= 5 then return end
    activeNotifs += 1

    local notif = Instance.new("Frame")
    notif.Name = "Notif"
    notif.Size = UDim2.new(1, 0, 0, 42)
    notif.BackgroundColor3 = Color3.fromRGB(12, 12, 18)
    notif.BackgroundTransparency = 0.1
    notif.BorderSizePixel = 0
    notif.ZIndex = 4501
    notif.Parent = NotifContainer

    local notifCorner = Instance.new("UICorner")
    notifCorner.CornerRadius = UDim.new(0, 8)
    notifCorner.Parent = notif

    local notifGradient = Instance.new("UIGradient")
    notifGradient.Color = ColorSequence.new({
        ColorSequenceKeypoint.new(0, Color3.fromRGB(15, 15, 25)),
        ColorSequenceKeypoint.new(0.5, Color3.fromRGB(25, 30, 50)),
        ColorSequenceKeypoint.new(1, Color3.fromRGB(15, 15, 25)),
    })
    notifGradient.Rotation = 90
    notifGradient.Parent = notif

    local notifStroke = Instance.new("UIStroke")
    notifStroke.Thickness = 1.5
    notifStroke.Color = Colors.Accent
    notifStroke.Transparency = 0.3
    notifStroke.Parent = notif

    local accentBar = Instance.new("Frame")
    accentBar.Size = UDim2.new(0, 3, 1, 0)
    accentBar.Position = UDim2.new(0, 0, 0, 0)
    accentBar.BackgroundColor3 = Colors.Accent
    accentBar.BorderSizePixel = 0
    accentBar.Parent = notif

    local accentCorner = Instance.new("UICorner")
    accentCorner.CornerRadius = UDim.new(0, 8)
    accentCorner.Parent = accentBar

    local accentFix = Instance.new("Frame")
    accentFix.Size = UDim2.new(0, 4, 1, -8)
    accentFix.Position = UDim2.new(0, 0, 0, 4)
    accentFix.BackgroundColor3 = Colors.Accent
    accentFix.BorderSizePixel = 0
    accentFix.Parent = notif

    local dot = Instance.new("Frame")
    dot.Size = UDim2.new(0, 8, 0, 8)
    dot.Position = UDim2.new(0, 14, 0.5, -4)
    dot.BackgroundColor3 = Colors.Accent
    dot.BorderSizePixel = 0
    dot.Parent = notif

    local dotCorner = Instance.new("UICorner")
    dotCorner.CornerRadius = UDim.new(1, 0)
    dotCorner.Parent = dot

    local titleLabel = Instance.new("TextLabel")
    titleLabel.Text = title
    titleLabel.Size = UDim2.new(1, -40, 0, 18)
    titleLabel.Position = UDim2.new(0, 32, 0, 5)
    titleLabel.BackgroundTransparency = 1
    titleLabel.TextColor3 = Colors.Accent
    titleLabel.Font = FONT_BOLD
    titleLabel.TextSize = 12
    titleLabel.TextXAlignment = Enum.TextXAlignment.Left
    titleLabel.Parent = notif

    local subLabel = Instance.new("TextLabel")
    subLabel.Text = subtitle
    subLabel.Size = UDim2.new(1, -40, 0, 14)
    subLabel.Position = UDim2.new(0, 32, 0, 23)
    subLabel.BackgroundTransparency = 1
    subLabel.TextColor3 = Color3.fromRGB(180, 180, 200)
    subLabel.Font = FONT
    subLabel.TextSize = 11
    subLabel.TextXAlignment = Enum.TextXAlignment.Left
    subLabel.Parent = notif

    notif.Position = UDim2.new(0, -320, 0, 0)
    notif.BackgroundTransparency = 1
    notifStroke.Transparency = 1
    titleLabel.TextTransparency = 1
    subLabel.TextTransparency = 1
    accentBar.BackgroundTransparency = 1
    accentFix.BackgroundTransparency = 1
    dot.BackgroundTransparency = 1

    TweenService:Create(notif, TweenInfo.new(0.35, Enum.EasingStyle.Back, Enum.EasingDirection.Out), {
        Position = UDim2.new(0, 0, 0, 0),
        BackgroundTransparency = 0.1,
    }):Play()
    TweenService:Create(notifStroke, TweenInfo.new(0.35), {Transparency = 0.3}):Play()
    TweenService:Create(titleLabel, TweenInfo.new(0.35), {TextTransparency = 0}):Play()
    TweenService:Create(subLabel, TweenInfo.new(0.35), {TextTransparency = 0}):Play()
    TweenService:Create(accentBar, TweenInfo.new(0.35), {BackgroundTransparency = 0}):Play()
    TweenService:Create(accentFix, TweenInfo.new(0.35), {BackgroundTransparency = 0}):Play()
    TweenService:Create(dot, TweenInfo.new(0.35), {BackgroundTransparency = 0}):Play()

    task.spawn(function()
        while notif.Parent do
            TweenService:Create(dot, TweenInfo.new(0.6, Enum.EasingStyle.Sine), {
                Size = UDim2.new(0, 10, 0, 10),
                Position = UDim2.new(0, 13, 0.5, -5),
            }):Play()
            task.wait(0.6)
            TweenService:Create(dot, TweenInfo.new(0.6, Enum.EasingStyle.Sine), {
                Size = UDim2.new(0, 8, 0, 8),
                Position = UDim2.new(0, 14, 0.5, -4),
            }):Play()
            task.wait(0.6)
        end
    end)

    task.spawn(function()
        task.wait(2.5)
        TweenService:Create(notif, TweenInfo.new(0.35, Enum.EasingStyle.Quad, Enum.EasingDirection.In), {
            Position = UDim2.new(0, -320, 0, 0),
            BackgroundTransparency = 1,
        }):Play()
        TweenService:Create(notifStroke, TweenInfo.new(0.35), {Transparency = 1}):Play()
        TweenService:Create(titleLabel, TweenInfo.new(0.3), {TextTransparency = 1}):Play()
        TweenService:Create(subLabel, TweenInfo.new(0.3), {TextTransparency = 1}):Play()
        TweenService:Create(accentBar, TweenInfo.new(0.3), {BackgroundTransparency = 1}):Play()
        TweenService:Create(accentFix, TweenInfo.new(0.3), {BackgroundTransparency = 1}):Play()
        TweenService:Create(dot, TweenInfo.new(0.3), {BackgroundTransparency = 1}):Play()
        task.wait(0.4)
        notif:Destroy()
        activeNotifs -= 1
    end)
end

-----------------------------------------------------------
-- // WATERMARK
-----------------------------------------------------------
local Watermark = Instance.new("Frame")
Watermark.Name = "Watermark"
Watermark.Size = UDim2.new(0, 460, 0, 38)
Watermark.Position = UDim2.new(0.5, -230, 0, 15)
Watermark.BackgroundColor3 = Color3.fromRGB(12, 12, 16)
Watermark.BackgroundTransparency = 0.05
Watermark.BorderSizePixel = 0
Watermark.ZIndex = 5000
Watermark.Parent = ScreenGui

local WmCorner = Instance.new("UICorner")
WmCorner.CornerRadius = UDim.new(0, 10)
WmCorner.Parent = Watermark

local WmGradient = Instance.new("UIGradient")
WmGradient.Color = ColorSequence.new({
    ColorSequenceKeypoint.new(0, Color3.fromRGB(20,20,30)),
    ColorSequenceKeypoint.new(0.5, Color3.fromRGB(40,40,60)),
    ColorSequenceKeypoint.new(1, Color3.fromRGB(20,20,30)),
})
WmGradient.Rotation = 90
WmGradient.Parent = Watermark

local WmStroke = Instance.new("UIStroke")
WmStroke.Thickness = 1.5
WmStroke.Color = Color3.fromRGB(80, 120, 255)
WmStroke.Transparency = 0.2
WmStroke.Parent = Watermark

local WmStrokeGradient = Instance.new("UIGradient")
WmStrokeGradient.Color = ColorSequence.new({
    ColorSequenceKeypoint.new(0, Colors.Accent),
    ColorSequenceKeypoint.new(0.5, Colors.Accent2),
    ColorSequenceKeypoint.new(1, Colors.Accent),
})
WmStrokeGradient.Parent = WmStroke

local WmIcon = Instance.new("Frame")
WmIcon.Size = UDim2.new(0, 22, 0, 22)
WmIcon.Position = UDim2.new(0, 10, 0.5, -11)
WmIcon.BackgroundColor3 = Colors.Accent
WmIcon.BorderSizePixel = 0
WmIcon.Parent = Watermark

local WmIconCorner = Instance.new("UICorner")
WmIconCorner.CornerRadius = UDim.new(1, 0)
WmIconCorner.Parent = WmIcon

local WmIconDot = Instance.new("Frame")
WmIconDot.Size = UDim2.new(0.5, 0, 0.5, 0)
WmIconDot.Position = UDim2.new(0.25, 0, 0.25, 0)
WmIconDot.BackgroundColor3 = Color3.fromRGB(255,255,255)
WmIconDot.BorderSizePixel = 0
WmIconDot.Parent = WmIcon

local WmIconDotCorner = Instance.new("UICorner")
WmIconDotCorner.CornerRadius = UDim.new(1, 0)
WmIconDotCorner.Parent = WmIconDot

local WmIconRing = Instance.new("Frame")
WmIconRing.Size = UDim2.new(0, 30, 0, 30)
WmIconRing.Position = UDim2.new(0, 6, 0.5, -15)
WmIconRing.BackgroundTransparency = 1
WmIconRing.Parent = Watermark

local WmIconRingCorner = Instance.new("UICorner")
WmIconRingCorner.CornerRadius = UDim.new(1, 0)
WmIconRingCorner.Parent = WmIconRing

local WmIconRingStroke = Instance.new("UIStroke")
WmIconRingStroke.Thickness = 1.5
WmIconRingStroke.Color = Colors.Accent
WmIconRingStroke.Transparency = 0.5
WmIconRingStroke.Parent = WmIconRing

local function createWmSection(text, xPos, width, align, isAccent)
    local sec = Instance.new("Frame")
    sec.Size = UDim2.new(0, width, 1, 0)
    sec.Position = UDim2.new(0, xPos, 0, 0)
    sec.BackgroundTransparency = 1
    sec.Parent = Watermark

    local label = Instance.new("TextLabel")
    label.Size = UDim2.new(1, 0, 1, 0)
    label.BackgroundTransparency = 1
    label.Text = text
    label.TextColor3 = isAccent and Colors.Accent or Colors.Text
    label.Font = FONT_BOLD
    label.TextSize = 13
    label.TextXAlignment = align or Enum.TextXAlignment.Center
    label.Parent = sec

    return label
end

local function createDivider(xPos)
    local div = Instance.new("Frame")
    div.Size = UDim2.new(0, 1, 0, 20)
    div.Position = UDim2.new(0, xPos, 0.5, -10)
    div.BackgroundColor3 = Color3.fromRGB(80, 80, 100)
    div.BackgroundTransparency = 0.4
    div.BorderSizePixel = 0
    div.Parent = Watermark
end

local nameLabel = createWmSection(player.Name, 45, 170, Enum.TextXAlignment.Left, true)
local pingLabel = createWmSection("0 ms", 220, 110, Enum.TextXAlignment.Center, false)
local speedLabel = createWmSection("0 spd", 335, 110, Enum.TextXAlignment.Center, false)

createDivider(215)
createDivider(330)

task.spawn(function()
    while Watermark.Parent do
        TweenService:Create(WmIconRing, TweenInfo.new(0.9, Enum.EasingStyle.Sine), {
            Size = UDim2.new(0, 36, 0, 36),
            Position = UDim2.new(0, 3, 0.5, -18),
        }):Play()
        TweenService:Create(WmIconRingStroke, TweenInfo.new(0.9), {Transparency = 0.8}):Play()
        task.wait(0.9)
        TweenService:Create(WmIconRing, TweenInfo.new(0.9, Enum.EasingStyle.Sine), {
            Size = UDim2.new(0, 30, 0, 30),
            Position = UDim2.new(0, 6, 0.5, -15),
        }):Play()
        TweenService:Create(WmIconRingStroke, TweenInfo.new(0.9), {Transparency = 0.3}):Play()
        task.wait(0.9)
    end
end)

task.spawn(function()
    while Watermark.Parent do
        local ping = 0
        pcall(function()
            ping = math.floor(Stats.Network.ServerStatsItem["Data Ping"]:GetValue())
        end)
        pingLabel.Text = ping .. " ms"

        local speed = 0
        local char = player.Character
        if char then
            local hrp = char:FindFirstChild("HumanoidRootPart")
            if hrp then
                local vel = hrp.AssemblyLinearVelocity
                speed = math.floor(math.sqrt(vel.X^2 + vel.Z^2))
            end
        end
        speedLabel.Text = speed .. " spd"

        task.wait(0.5)
    end
end)

-----------------------------------------------------------
-- // ОПРЕДЕЛЕНИЕ КОМАНДЫ
-----------------------------------------------------------
local function getTeamName(targetPlayer)
    local function check(str)
        if typeof(str) ~= "string" then return nil end
        local n = string.lower(str)
        if string.find(n, "spectat") or string.find(n, "spec") then return "spectator" end
        if string.find(n, "killer") or string.find(n, "kill") then return "killer" end
        if string.find(n, "surviv") or string.find(n, "surv") then return "survivor" end
        return nil
    end

    if targetPlayer.Team then
        local r = check(targetPlayer.Team.Name)
        if r then return r end
    end

    for _, attrName in ipairs({"Team", "team", "Role", "role", "GameTeam", "gameteam", "RoleName", "rolename", "TeamName", "teamname"}) do
        local a = targetPlayer:GetAttribute(attrName)
        local r = check(a)
        if r then return r end
    end

    local ls = targetPlayer:FindFirstChild("leaderstats")
    if ls then
        for _, child in ipairs(ls:GetChildren()) do
            local r = check(child.Name)
            if r then return r end
            if child:IsA("StringValue") or child:IsA("ObjectValue") then
                local r2 = check(child.Value and tostring(child.Value) or "")
                if r2 then return r2 end
            end
        end
    end

    local char = targetPlayer.Character
    if char then
        for _, obj in ipairs(char:GetDescendants()) do
            local r = check(obj.Name)
            if r then return r end
        end
    end

    for _, child in ipairs(targetPlayer:GetChildren()) do
        local r = check(child.Name)
        if r then return r end
        if child:IsA("StringValue") or child:IsA("ObjectValue") then
            local r2 = check(child.Value and tostring(child.Value) or "")
            if r2 then return r2 end
        end
    end

    return nil
end

-----------------------------------------------------------
-- // ESP ДЛЯ ИГРОКОВ (СТАРЫЙ)
-----------------------------------------------------------
local Settings = {
    KillerColor1 = Color3.fromRGB(80, 0, 0),
    KillerColor2 = Color3.fromRGB(255, 40, 40),
    SurvivorColor1 = Color3.fromRGB(0, 0, 70),
    SurvivorColor2 = Color3.fromRGB(0, 150, 255),
    KillerParticle = Color3.fromRGB(255, 60, 60),
    SurvivorParticle = Color3.fromRGB(0, 100, 255),
    SpawnRate = 0.15,
    BoxThickness = 3.5,
    RotationSpeed = 3,
    MaxDistance = 2500,
}

local ParticleFolder = Instance.new("Folder", workspace)
ParticleFolder.Name = "ESP_DarkBlue_Particles"

local espInstances = {}
local espConnections = {}
local espPerPlayer = {}

local function getTeamColors(targetPlayer)
    local team = getTeamName(targetPlayer)
    if team == "survivor" then
        return Settings.SurvivorColor1, Settings.SurvivorColor2, Settings.SurvivorParticle
    end
    if team == "killer" then
        return Settings.KillerColor1, Settings.KillerColor2, Settings.KillerParticle
    end
    return Color3.fromRGB(60,60,60), Color3.fromRGB(160,160,160), Color3.fromRGB(120,120,120)
end

local function RemoveVanillaName(char)
    local humanoid = char:FindFirstChildOfClass("Humanoid")
    if humanoid then
        humanoid.DisplayDistanceType = Enum.HumanoidDisplayDistanceType.None
    end
end

local function SpawnNeonParticle(pos, particleColor)
    local part = Instance.new("Part")
    part.Size = Vector3.new(0.3, 0.3, 0.3)
    part.Position = pos + Vector3.new(math.random(-2,2), -3.5, math.random(-2,2))
    part.Anchored = true
    part.CanCollide = false
    part.Shape = Enum.PartType.Ball
    part.Material = Enum.Material.Neon
    part.Color = particleColor
    part.Parent = ParticleFolder

    local tween = TweenService:Create(part,
        TweenInfo.new(1.8, Enum.EasingStyle.Quart),
        { Position = part.Position + Vector3.new(0, 7, 0), Transparency = 1, Size = Vector3.new(0,0,0) }
    )
    tween:Play()
    Debris:AddItem(part, 1.8)
end

local function createESPForChar(targetPlayer, char)
    if not char or not char.Parent then return end
    if getTeamName(targetPlayer) == "spectator" then return end
    if espPerPlayer[targetPlayer] then return end

    RemoveVanillaName(char)

    local root = char:WaitForChild("HumanoidRootPart", 15)
    if not root then return end

    local playerName = targetPlayer.Name
    local userId = targetPlayer.UserId
    local color1, color2, particleColor = getTeamColors(targetPlayer)

    local boxGui = Instance.new("BillboardGui")
    boxGui.Name = "delux_ESPBox"
    boxGui.Adornee = root
    boxGui.Size = UDim2.new(4,0,5.5,0)
    boxGui.AlwaysOnTop = true
    boxGui.MaxDistance = Settings.MaxDistance
    boxGui.Parent = player:WaitForChild("PlayerGui")

    local mainFrame = Instance.new("Frame")
    mainFrame.Size = UDim2.new(1,0,1,0)
    mainFrame.BackgroundTransparency = 1
    mainFrame.Parent = boxGui

    local corner = Instance.new("UICorner")
    corner.CornerRadius = UDim.new(0.3, 0)
    corner.Parent = mainFrame

    local stroke = Instance.new("UIStroke")
    stroke.Thickness = Settings.BoxThickness
    stroke.Color = Color3.new(1,1,1)
    stroke.ApplyStrokeMode = Enum.ApplyStrokeMode.Border
    stroke.Parent = mainFrame

    local gradient = Instance.new("UIGradient")
    gradient.Color = ColorSequence.new({
        ColorSequenceKeypoint.new(0, color1),
        ColorSequenceKeypoint.new(0.5, color2),
        ColorSequenceKeypoint.new(1, color1)
    })
    gradient.Parent = stroke

    local rotConn = RunService.RenderStepped:Connect(function()
        if not boxGui.Parent then rotConn:Disconnect(); return end
        gradient.Rotation += Settings.RotationSpeed
    end)
    table.insert(espConnections, rotConn)

    local nameGui = Instance.new("BillboardGui")
    nameGui.Name = "delux_ESPName"
    nameGui.Adornee = root
    nameGui.Size = UDim2.new(4,0,1.5,0)
    nameGui.StudsOffset = Vector3.new(0, 3.5, 0)
    nameGui.AlwaysOnTop = true
    nameGui.MaxDistance = Settings.MaxDistance
    nameGui.Parent = player:WaitForChild("PlayerGui")

    local container = Instance.new("Frame")
    container.Size = UDim2.new(1,0,1,0)
    container.BackgroundTransparency = 1
    container.Parent = nameGui

    local avatar = Instance.new("ImageLabel")
    avatar.Size = UDim2.new(0.25,0,1,0)
    avatar.BackgroundTransparency = 1
    avatar.Parent = container

    task.spawn(function()
        local ok2, img = pcall(function()
            return Players:GetUserThumbnailAsync(userId,
                Enum.ThumbnailType.HeadShot,
                Enum.ThumbnailSize.Size150x150)
        end)
        if ok2 then avatar.Image = img end
    end)

    local nameLabel = Instance.new("TextLabel")
    nameLabel.Size = UDim2.new(0.75,0,1,0)
    nameLabel.Position = UDim2.new(0.27,0,0,0)
    nameLabel.BackgroundTransparency = 1
    nameLabel.Text = playerName
    nameLabel.TextScaled = true
    nameLabel.TextColor3 = color2
    nameLabel.TextStrokeTransparency = 0.3
    nameLabel.Font = FONT_BOLD
    nameLabel.Parent = container

    pcall(function()
        nameLabel.FontFace = Font.new("rbxassetid://11322590111")
    end)

    task.spawn(function()
        while boxGui.Parent do
            local dist = (Camera.CFrame.Position - root.Position).Magnitude
            if dist < 300 then
                SpawnNeonParticle(root.Position, particleColor)
            end
            task.wait(Settings.SpawnRate)
        end
    end)

    table.insert(espInstances, boxGui)
    table.insert(espInstances, nameGui)

    espPerPlayer[targetPlayer] = {boxGui = boxGui, nameGui = nameGui}

    local conn
    conn = RunService.RenderStepped:Connect(function()
        if not char.Parent then
            boxGui:Destroy()
            nameGui:Destroy()
            conn:Disconnect()
            espPerPlayer[targetPlayer] = nil
        end
    end)
    table.insert(espConnections, conn)
end

local function startESP()
    for _, p in pairs(Players:GetPlayers()) do
        if p ~= player then
            if p.Character then
                task.spawn(createESPForChar, p, p.Character)
            end
            local c
            c = p.CharacterAdded:Connect(function(ch)
                if getgenv().ESPEnabled then
                    task.wait(0.3)
                    createESPForChar(p, ch)
                end
            end)
            table.insert(espConnections, c)
        end
    end

    local pac
    pac = Players.PlayerAdded:Connect(function(p)
        if p == player then return end
        p.CharacterAdded:Connect(function(ch)
            if getgenv().ESPEnabled then
                task.wait(0.3)
                createESPForChar(p, ch)
            end
        end)
    end)
    table.insert(espConnections, pac)
end

local function stopESP()
    for _, inst in ipairs(espInstances) do
        if inst and inst.Parent then inst:Destroy() end
    end
    espInstances = {}
    espPerPlayer = {}
    for _, c in ipairs(espConnections) do
        if c then pcall(function() c:Disconnect() end) end
    end
    espConnections = {}
    for _, part in ipairs(ParticleFolder:GetChildren()) do part:Destroy() end
end

player.CharacterAdded:Connect(function()
    if getgenv().ESPEnabled then
        task.wait(0.5)
        startESP()
    end
end)

-----------------------------------------------------------
-- // ESP 2.0
-----------------------------------------------------------
local esp2Tracked = {}
local esp2Connections = {}

local function createESP2(targetPlayer, char)
    if not char or not char.Parent then return end
    if esp2Tracked[targetPlayer] then return end
    if getTeamName(targetPlayer) == "spectator" then return end

    local team = getTeamName(targetPlayer)
    local color
    if team == "killer" then
        color = Colors.KillerRed
    else
        color = Colors.SurvivorGreen
    end

    local highlight = Instance.new("Highlight")
    highlight.Name = "delux_ESP2Highlight"
    highlight.Adornee = char
    highlight.FillColor = color
    highlight.FillTransparency = 0.75
    highlight.OutlineColor = color
    highlight.OutlineTransparency = 0
    highlight.DepthMode = Enum.HighlightDepthMode.AlwaysOnTop
    highlight.Parent = player:WaitForChild("PlayerGui")

    esp2Tracked[targetPlayer] = {highlight = highlight}

    local conn
    conn = RunService.RenderStepped:Connect(function()
        if not char.Parent then
            if highlight and highlight.Parent then highlight:Destroy() end
            conn:Disconnect()
            esp2Tracked[targetPlayer] = nil
        end
    end)
    table.insert(esp2Connections, conn)
end

local function startESP2()
    for _, p in pairs(Players:GetPlayers()) do
        if p ~= player then
            if p.Character then
                task.spawn(createESP2, p, p.Character)
            end
            local c
            c = p.CharacterAdded:Connect(function(ch)
                if getgenv().ESP2Enabled then
                    task.wait(0.3)
                    createESP2(p, ch)
                end
            end)
            table.insert(esp2Connections, c)
        end
    end

    local pac
    pac = Players.PlayerAdded:Connect(function(p)
        if p == player then return end
        p.CharacterAdded:Connect(function(ch)
            if getgenv().ESP2Enabled then
                task.wait(0.3)
                createESP2(p, ch)
            end
        end)
    end)
    table.insert(esp2Connections, pac)
end

local function stopESP2()
    for p, data in pairs(esp2Tracked) do
        if data.highlight and data.highlight.Parent then data.highlight:Destroy() end
    end
    esp2Tracked = {}
    for _, c in ipairs(esp2Connections) do
        if typeof(c) == "RBXScriptConnection" then pcall(function() c:Disconnect() end) end
    end
    esp2Connections = {}
end

player.CharacterAdded:Connect(function()
    if getgenv().ESP2Enabled then
        task.wait(0.5)
        startESP2()
    end
end)

-----------------------------------------------------------
-- // ESP GENERATOR
-----------------------------------------------------------
local genTracked = {}

local function isGenerator(obj)
    if not obj or (not obj:IsA("Model") and not obj:IsA("BasePart")) then return false end
    local n = string.lower(obj.Name)
    if string.find(n, "generator") then return true end
    return false
end

local function createGenESP(model)
    if genTracked[model] then return end
    if not model.Parent then return end
    local primary = model:IsA("Model") and (model.PrimaryPart or model:FindFirstChildWhichIsA("BasePart")) or model
    if not primary then return end

    local highlight = Instance.new("Highlight")
    highlight.Name = "delux_GenHighlight"
    highlight.Adornee = model
    highlight.FillColor = Colors.GenGreen
    highlight.FillTransparency = 0.8
    highlight.OutlineColor = Colors.GenGreen
    highlight.OutlineTransparency = 0
    highlight.DepthMode = Enum.HighlightDepthMode.AlwaysOnTop
    highlight.Parent = player:WaitForChild("PlayerGui")

    local nameGui = Instance.new("BillboardGui")
    nameGui.Name = "delux_GenName"
    nameGui.Adornee = primary
    nameGui.Size = UDim2.new(4, 0, 1, 0)
    nameGui.StudsOffsetWorldSpace = Vector3.new(0, 4, 0)
    nameGui.AlwaysOnTop = true
    nameGui.MaxDistance = 5000
    nameGui.Parent = player:WaitForChild("PlayerGui")

    local label = Instance.new("TextLabel")
    label.Size = UDim2.new(1, 0, 1, 0)
    label.BackgroundTransparency = 1
    label.Text = "[GEN] " .. model.Name
    label.TextScaled = true
    label.TextColor3 = Colors.GenGreen
    label.TextStrokeTransparency = 0.3
    label.Font = FONT_BOLD
    label.Parent = nameGui

    genTracked[model] = {highlight = highlight, nameGui = nameGui}
end

local genESPConnections = {}

local function startGenESP()
    for _, obj in ipairs(workspace:GetDescendants()) do
        if isGenerator(obj) then createGenESP(obj) end
    end
    local addConn = workspace.DescendantAdded:Connect(function(obj)
        if getgenv().GenESPEnabled and isGenerator(obj) then
            task.wait(0.1); createGenESP(obj)
        end
    end)
    table.insert(genESPConnections, addConn)
    local remConn = workspace.DescendantRemoving:Connect(function(obj)
        if genTracked[obj] then
            local data = genTracked[obj]
            if data.highlight then data.highlight:Destroy() end
            if data.nameGui then data.nameGui:Destroy() end
            genTracked[obj] = nil
        end
    end)
    table.insert(genESPConnections, remConn)
end

local function stopGenESP()
    for model, data in pairs(genTracked) do
        if data.highlight then data.highlight:Destroy() end
        if data.nameGui then data.nameGui:Destroy() end
    end
    genTracked = {}
    for _, c in ipairs(genESPConnections) do
        if typeof(c) == "RBXScriptConnection" then pcall(function() c:Disconnect() end) end
    end
    genESPConnections = {}
end

-----------------------------------------------------------
-- // ESP PALLET
-----------------------------------------------------------
local palletTracked = {}

local function isPallet(obj)
    if not obj or (not obj:IsA("Model") and not obj:IsA("BasePart")) then return false end
    local n = string.lower(obj.Name)
    if string.find(n, "pallet") then return true end
    return false
end

local function createPalletESP(model)
    if palletTracked[model] then return end
    if not model.Parent then return end
    local primary = model:IsA("Model") and (model.PrimaryPart or model:FindFirstChildWhichIsA("BasePart")) or model
    if not primary then return end

    local highlight = Instance.new("Highlight")
    highlight.Name = "delux_PalletHighlight"
    highlight.Adornee = model
    highlight.FillColor = Colors.PalletOrange
    highlight.FillTransparency = 0.8
    highlight.OutlineColor = Colors.PalletOrange
    highlight.OutlineTransparency = 0
    highlight.DepthMode = Enum.HighlightDepthMode.AlwaysOnTop
    highlight.Parent = player:WaitForChild("PlayerGui")

    local nameGui = Instance.new("BillboardGui")
    nameGui.Name = "delux_PalletName"
    nameGui.Adornee = primary
    nameGui.Size = UDim2.new(4, 0, 1, 0)
    nameGui.StudsOffsetWorldSpace = Vector3.new(0, 4, 0)
    nameGui.AlwaysOnTop = true
    nameGui.MaxDistance = 5000
    nameGui.Parent = player:WaitForChild("PlayerGui")

    local label = Instance.new("TextLabel")
    label.Size = UDim2.new(1, 0, 1, 0)
    label.BackgroundTransparency = 1
    label.Text = "[PALLET] " .. model.Name
    label.TextScaled = true
    label.TextColor3 = Colors.PalletOrange
    label.TextStrokeTransparency = 0.3
    label.Font = FONT_BOLD
    label.Parent = nameGui

    palletTracked[model] = {highlight = highlight, nameGui = nameGui}
end

local palletESPConnections = {}

local function startPalletESP()
    for _, obj in ipairs(workspace:GetDescendants()) do
        if isPallet(obj) then createPalletESP(obj) end
    end
    local addConn = workspace.DescendantAdded:Connect(function(obj)
        if getgenv().PalletESPEnabled and isPallet(obj) then
            task.wait(0.1); createPalletESP(obj)
        end
    end)
    table.insert(palletESPConnections, addConn)
    local remConn = workspace.DescendantRemoving:Connect(function(obj)
        if palletTracked[obj] then
            local data = palletTracked[obj]
            if data.highlight then data.highlight:Destroy() end
            if data.nameGui then data.nameGui:Destroy() end
            palletTracked[obj] = nil
        end
    end)
    table.insert(palletESPConnections, remConn)
end

local function stopPalletESP()
    for model, data in pairs(palletTracked) do
        if data.highlight then data.highlight:Destroy() end
        if data.nameGui then data.nameGui:Destroy() end
    end
    palletTracked = {}
    for _, c in ipairs(palletESPConnections) do
        if typeof(c) == "RBXScriptConnection" then pcall(function() c:Disconnect() end) end
    end
    palletESPConnections = {}
end

-----------------------------------------------------------
-- // FULLBRIGHT
-----------------------------------------------------------
local originalLighting = {
    Brightness = Lighting.Brightness,
    ClockTime = Lighting.ClockTime,
    GlobalShadows = Lighting.GlobalShadows,
    Ambient = Lighting.Ambient,
    OutdoorAmbient = Lighting.OutdoorAmbient,
}

local function applyFullbright()
    if not getgenv().FullbrightEnabled then return end
    local v = getgenv().FullbrightValue
    Lighting.Brightness = originalLighting.Brightness + v * 1.5
    Lighting.ClockTime = 14
    Lighting.GlobalShadows = false
    Lighting.Ambient = Color3.fromRGB(120, 120, 120)
    Lighting.OutdoorAmbient = Color3.fromRGB(160, 160, 160)
end

local function restoreLighting()
    Lighting.Brightness = originalLighting.Brightness
    Lighting.ClockTime = originalLighting.ClockTime
    Lighting.GlobalShadows = originalLighting.GlobalShadows
    Lighting.Ambient = originalLighting.Ambient
    Lighting.OutdoorAmbient = originalLighting.OutdoorAmbient
end

RunService.RenderStepped:Connect(function()
    if getgenv().FullbrightEnabled then applyFullbright() end
end)

-----------------------------------------------------------
-- // LIGHTBORN (защита от ослепления фонариком/вспышками)
-----------------------------------------------------------
local lightbornStored = {}
local lightbornConnection = nil

local function isLightOurs(light)
    local char = player.Character
    if not char then return false end
    local parent = light.Parent
    if not parent then return false end
    if parent:IsDescendantOf(char) then return true end
    return false
end

local function storeOriginal(obj)
    if lightbornStored[obj] then return end

    if obj:IsA("Light") then
        lightbornStored[obj] = {
            type = "Light",
            Brightness = obj.Brightness,
            Enabled = obj.Enabled,
            Range = obj.Range,
        }
    elseif obj:IsA("Highlight") then
        lightbornStored[obj] = {
            type = "Highlight",
            FillTransparency = obj.FillTransparency,
            OutlineTransparency = obj.OutlineTransparency,
            Enabled = obj.Enabled,
        }
    elseif obj:IsA("BloomEffect") then
        lightbornStored[obj] = {
            type = "BloomEffect",
            Intensity = obj.Intensity,
            Enabled = obj.Enabled,
        }
    elseif obj:IsA("ColorCorrectionEffect") then
        lightbornStored[obj] = {
            type = "ColorCorrectionEffect",
            Brightness = obj.Brightness,
            Contrast = obj.Contrast,
            Enabled = obj.Enabled,
        }
    elseif obj:IsA("ImageLabel") or obj:IsA("Frame") then
        lightbornStored[obj] = {
            type = "Gui",
            Visible = obj.Visible,
            BackgroundTransparency = obj.BackgroundTransparency,
            ImageTransparency = obj.ImageTransparency or 0,
        }
    end
end

local function applyLightborn()
    -- 1. Свет
    for _, obj in ipairs(workspace:GetDescendants()) do
        if obj:IsA("Light") then
            if not isLightOurs(obj) then
                local isWhiteish = (obj.Color.R > 0.8 and obj.Color.G > 0.8 and obj.Color.B > 0.8)
                local isBright = obj.Brightness >= 2
                local isSpotlight = obj:IsA("SpotLight") and obj.Range > 15
                local isPointlight = obj:IsA("PointLight") and obj.Range > 15
                local isSurface = obj:IsA("SurfaceLight")

                if isWhiteish or isBright or isSpotlight or isPointlight or isSurface then
                    storeOriginal(obj)
                    obj.Enabled = false
                end
            end
        end
    end

    -- 2. Highlight
    for _, obj in ipairs(workspace:GetDescendants()) do
        if obj:IsA("Highlight") then
            if obj.FillTransparency < 0.5 or obj.OutlineTransparency < 0.3 then
                storeOriginal(obj)
                obj.Enabled = false
            end
        end
    end

    -- 3. Эффекты в Lighting
    for _, obj in ipairs(Lighting:GetChildren()) do
        if obj:IsA("BloomEffect") then
            storeOriginal(obj)
            obj.Enabled = false
        elseif obj:IsA("ColorCorrectionEffect") then
            if obj.Brightness > 0.05 or obj.Contrast > 0.1 then
                storeOriginal(obj)
                obj.Enabled = false
            end
        end
    end

    -- 4. GUI-вспышки
    pcall(function()
        for _, gui in ipairs(player.PlayerGui:GetDescendants()) do
            if (gui:IsA("ImageLabel") or gui:IsA("Frame")) then
                local isWhite = (gui.BackgroundColor3.R > 0.9 and gui.BackgroundColor3.G > 0.9 and gui.BackgroundColor3.B > 0.9)
                local isBig = gui.AbsoluteSize.X > 400 and gui.AbsoluteSize.Y > 300
                local isOnTop = gui.ZIndex > 20
                local hasWhiteImage = false
                if gui:IsA("ImageLabel") and gui.ImageColor3 then
                    hasWhiteImage = (gui.ImageColor3.R > 0.9 and gui.ImageColor3.G > 0.9 and gui.ImageColor3.B > 0.9)
                end

                if (isWhite or hasWhiteImage) and isBig and isOnTop then
                    storeOriginal(gui)
                    gui.Visible = false
                end
            end
        end
    end)
end

local function enableLightborn()
    if lightbornConnection then return end
    lightbornStored = {}

    lightbornConnection = RunService.RenderStepped:Connect(function()
        applyLightborn()
    end)
end

local function disableLightborn()
    if lightbornConnection then
        lightbornConnection:Disconnect()
        lightbornConnection = nil
    end

    for obj, data in pairs(lightbornStored) do
        if obj and obj.Parent then
            pcall(function()
                if data.type == "Light" then
                    obj.Brightness = data.Brightness
                    obj.Enabled = data.Enabled
                    obj.Range = data.Range
                elseif data.type == "Highlight" then
                    obj.FillTransparency = data.FillTransparency
                    obj.OutlineTransparency = data.OutlineTransparency
                    obj.Enabled = data.Enabled
                elseif data.type == "BloomEffect" then
                    obj.Intensity = data.Intensity
                    obj.Enabled = data.Enabled
                elseif data.type == "ColorCorrectionEffect" then
                    obj.Brightness = data.Brightness
                    obj.Contrast = data.Contrast
                    obj.Enabled = data.Enabled
                elseif data.type == "Gui" then
                    obj.Visible = data.Visible
                    obj.BackgroundTransparency = data.BackgroundTransparency
                    if obj:IsA("ImageLabel") then
                        obj.ImageTransparency = data.ImageTransparency
                    end
                end
            end)
        end
    end
    lightbornStored = {}
end

local function setLightborn(state)
    getgenv().LightbornEnabled = state
    if state then enableLightborn() else disableLightborn() end
end

-----------------------------------------------------------
-- // FOV
-----------------------------------------------------------
RunService.RenderStepped:Connect(function()
    local targetFov = getgenv().FOVValue or 70
    if Camera.FieldOfView ~= targetFov then
        Camera.FieldOfView = targetFov
    end
end)

-----------------------------------------------------------
-- // TP К КИЛЛЕРУ
-----------------------------------------------------------
local function findKiller()
    for _, p in pairs(Players:GetPlayers()) do
        if p ~= player and getTeamName(p) == "killer" then return p end
    end
    return nil
end

local function teleportBehindKiller()
    local killer = findKiller()
    if not killer then warn("[TP Killer] Киллер не найден!"); return end
    local kChar = killer.Character
    if not kChar then return end
    local kHRP = kChar:FindFirstChild("HumanoidRootPart")
    if not kHRP then return end
    local myChar = player.Character
    if not myChar then return end
    local myHRP = myChar:FindFirstChild("HumanoidRootPart")
    if not myHRP then return end

    local backDir = -kHRP.CFrame.LookVector
    local targetPos = kHRP.Position + backDir * 4
    myHRP.CFrame = CFrame.new(targetPos, targetPos + kHRP.CFrame.LookVector)
end

-----------------------------------------------------------
-- // NOCLIP
-----------------------------------------------------------
local noclipConn = nil

local function enableNoclip()
    if noclipConn then return end
    noclipConn = RunService.Stepped:Connect(function()
        local char = player.Character
        if not char then return end
        for _, part in ipairs(char:GetDescendants()) do
            if part:IsA("BasePart") and part.CanCollide then
                part.CanCollide = false
            end
        end
    end)
end

local function disableNoclip()
    if noclipConn then
        noclipConn:Disconnect()
        noclipConn = nil
    end
    local char = player.Character
    if char then
        for _, part in ipairs(char:GetDescendants()) do
            if part:IsA("BasePart") then
                part.CanCollide = true
            end
        end
    end
end

local function setNoclip(state)
    getgenv().NoclipEnabled = state
    if state then enableNoclip() else disableNoclip() end
end

player.CharacterAdded:Connect(function()
    task.wait(0.5)
    if getgenv().NoclipEnabled then enableNoclip() end
end)

-----------------------------------------------------------
-- // KEYBIND СИСТЕМА
-----------------------------------------------------------
getgenv().ListeningForKeybind = false
getgenv().BindTarget = nil

local function keyCodeToString(kc)
    if not kc then return "NONE" end
    return kc.Name
end

UserInputService.InputBegan:Connect(function(input, gp)
    if gp then return end

    if getgenv().ListeningForKeybind and getgenv().BindTarget then
        if input.KeyCode == Enum.KeyCode.Unknown then return end

        local bindName = ""
        if getgenv().BindTarget == "tp" then
            getgenv().TPKillerKeybind = input.KeyCode
            bindName = "TP KILLER"
        elseif getgenv().BindTarget == "walkspeed" then
            getgenv().WalkSpeedKeybind = input.KeyCode
            bindName = "WALKSPEED"
        elseif getgenv().BindTarget == "noclip" then
            getgenv().NoclipKeybind = input.KeyCode
            bindName = "NOCLIP"
        end

        getgenv().ListeningForKeybind = false
        getgenv().BindTarget = nil
        saveConfig()
        showNotification("KEYBIND SET", bindName .. " → " .. input.KeyCode.Name)
        return
    end

    if getgenv().TPKillerKeybind and input.KeyCode == getgenv().TPKillerKeybind then
        teleportBehindKiller()
        showNotification("TP KILLER", "Teleported behind killer")
    end

    if getgenv().WalkSpeedKeybind and input.KeyCode == getgenv().WalkSpeedKeybind then
        getgenv().WalkSpeedActive = not getgenv().WalkSpeedActive
        saveConfig()
        showNotification("WALKSPEED", getgenv().WalkSpeedActive and "Activated" or "Deactivated")
    end

    if getgenv().NoclipKeybind and input.KeyCode == getgenv().NoclipKeybind then
        setNoclip(not getgenv().NoclipEnabled)
        saveConfig()
        showNotification("NOCLIP", getgenv().NoclipEnabled and "Activated" or "Deactivated")
    end
end)

UserInputService.InputBegan:Connect(function(input, gp)
    if gp then return end
    if input.UserInputType == Enum.UserInputType.MouseButton2 and getgenv().ListeningForKeybind then
        getgenv().ListeningForKeybind = false
        getgenv().BindTarget = nil
    end
end)

-----------------------------------------------------------
-- // СИНИЙ ТУМАН
-----------------------------------------------------------
local origFog = {
    FogColor = Lighting.FogColor,
    FogStart = Lighting.FogStart,
    FogEnd = Lighting.FogEnd,
}

local blueFogAtmosphere = nil

local function createBlueFog()
    Lighting.FogColor = Color3.fromRGB(40, 80, 200)
    Lighting.FogStart = 0
    Lighting.FogEnd = 60
    if not blueFogAtmosphere then
        blueFogAtmosphere = Instance.new("Atmosphere")
        blueFogAtmosphere.Name = "delux_BlueFogAtmosphere"
        blueFogAtmosphere.Color = Color3.fromRGB(80, 130, 255)
        blueFogAtmosphere.Decay = Color3.fromRGB(60, 100, 200)
        blueFogAtmosphere.Density = 0.45
        blueFogAtmosphere.Glare = 0
        blueFogAtmosphere.Haze = 2.5
        blueFogAtmosphere.Offset = 0
        blueFogAtmosphere.Parent = Lighting
    end
end

local function destroyBlueFog()
    Lighting.FogColor = origFog.FogColor
    Lighting.FogStart = origFog.FogStart
    Lighting.FogEnd = origFog.FogEnd
    if blueFogAtmosphere and blueFogAtmosphere.Parent then blueFogAtmosphere:Destroy() end
    blueFogAtmosphere = nil
end

RunService.RenderStepped:Connect(function()
    if getgenv().BlueFogEnabled then
        Lighting.FogColor = Color3.fromRGB(40, 80, 200)
        Lighting.FogStart = 0
        Lighting.FogEnd = 60
        if not blueFogAtmosphere or not blueFogAtmosphere.Parent then
            blueFogAtmosphere = Instance.new("Atmosphere")
            blueFogAtmosphere.Name = "delux_BlueFogAtmosphere"
            blueFogAtmosphere.Color = Color3.fromRGB(80, 130, 255)
            blueFogAtmosphere.Decay = Color3.fromRGB(60, 100, 200)
            blueFogAtmosphere.Density = 0.45
            blueFogAtmosphere.Glare = 0
            blueFogAtmosphere.Haze = 2.5
            blueFogAtmosphere.Offset = 0
            blueFogAtmosphere.Parent = Lighting
        end
    end
end)

-----------------------------------------------------------
-- // COMBAT SYSTEM
-----------------------------------------------------------
local function applyCombat()
    local char = player.Character
    if not char then return end
    local hum = char:FindFirstChildOfClass("Humanoid")
    if not hum then return end

    local targetSpeed = 16
    if getgenv().WalkSpeedActive then
        targetSpeed = getgenv().WalkSpeedValue
    end
    if hum.WalkSpeed ~= targetSpeed then
        hum.WalkSpeed = targetSpeed
    end

    if hum.UseJumpPower then
        if hum.JumpPower ~= getgenv().JumpPowerValue then
            hum.JumpPower = getgenv().JumpPowerValue
        end
    else
        local newHeight = (getgenv().JumpPowerValue ^ 2) / (2 * workspace.Gravity)
        if math.abs(hum.JumpHeight - newHeight) > 0.1 then
            hum.JumpHeight = newHeight
        end
    end

    if getgenv().JumpEnabled then
        hum:SetStateEnabled(Enum.HumanoidStateType.Jumping, true)
    else
        hum:SetStateEnabled(Enum.HumanoidStateType.Jumping, false)
    end
end

RunService.RenderStepped:Connect(applyCombat)
player.CharacterAdded:Connect(function()
    task.wait(0.5)
    applyCombat()
end)

-----------------------------------------------------------
-- // МЕНЮ
-----------------------------------------------------------
local MainFrame = Instance.new("Frame")
MainFrame.Name = "MainFrame"
MainFrame.Size = UDim2.new(0, 600, 0, 400)
MainFrame.Position = UDim2.new(0.5, -300, 0.5, -200)
MainFrame.BackgroundColor3 = Colors.Background
MainFrame.BorderSizePixel = 0
MainFrame.Active = true
MainFrame.Draggable = true
MainFrame.Visible = true
MainFrame.Parent = ScreenGui

local UICorner = Instance.new("UICorner")
UICorner.CornerRadius = UDim.new(0, 8)
UICorner.Parent = MainFrame

local Header = Instance.new("Frame")
Header.Size = UDim2.new(1, 0, 0, 40)
Header.BackgroundColor3 = Colors.Background
Header.BorderSizePixel = 0
Header.Parent = MainFrame

local HeaderCorner = Instance.new("UICorner")
HeaderCorner.CornerRadius = UDim.new(0, 8)
HeaderCorner.Parent = Header

local Title = Instance.new("TextLabel")
Title.Text = "delux.cc"
Title.Size = UDim2.new(1, 0, 1, 0)
Title.BackgroundTransparency = 1
Title.TextColor3 = Colors.Text
Title.Font = FONT_BOLD
Title.TextSize = 14
Title.TextXAlignment = Enum.TextXAlignment.Center
Title.Parent = Header

local MinimizeBtn = Instance.new("TextButton")
MinimizeBtn.Text = "—"
MinimizeBtn.Size = UDim2.new(0, 30, 0, 30)
MinimizeBtn.Position = UDim2.new(1, -35, 0, 5)
MinimizeBtn.BackgroundTransparency = 1
MinimizeBtn.TextColor3 = Colors.TextDim
MinimizeBtn.Font = FONT_BOLD
MinimizeBtn.TextSize = 14
MinimizeBtn.Parent = Header

local Sidebar = Instance.new("Frame")
Sidebar.Size = UDim2.new(0, 140, 1, -40)
Sidebar.Position = UDim2.new(0, 0, 0, 40)
Sidebar.BackgroundColor3 = Colors.Sidebar
Sidebar.BorderSizePixel = 0
Sidebar.Parent = MainFrame

local SidebarCorner = Instance.new("UICorner")
SidebarCorner.CornerRadius = UDim.new(0, 8)
SidebarCorner.Parent = Sidebar

local VisualBtn = Instance.new("TextButton")
VisualBtn.Size = UDim2.new(0.9, 0, 0, 35)
VisualBtn.Position = UDim2.new(0.05, 0, 0, 10)
VisualBtn.BackgroundColor3 = Colors.Element
VisualBtn.BorderSizePixel = 0
VisualBtn.Text = ""
VisualBtn.AutoButtonColor = false
VisualBtn.Parent = Sidebar

local VisualCorner = Instance.new("UICorner")
VisualCorner.CornerRadius = UDim.new(0, 6)
VisualCorner.Parent = VisualBtn

local VisualText = Instance.new("TextLabel")
VisualText.Text = "Visual"
VisualText.Size = UDim2.new(1, 0, 1, 0)
VisualText.BackgroundTransparency = 1
VisualText.TextColor3 = Colors.Text
VisualText.Font = FONT_BOLD
VisualText.TextSize = 13
VisualText.Parent = VisualBtn

local CombatBtn = Instance.new("TextButton")
CombatBtn.Size = UDim2.new(0.9, 0, 0, 35)
CombatBtn.Position = UDim2.new(0.05, 0, 0, 55)
CombatBtn.BackgroundColor3 = Colors.Sidebar
CombatBtn.BorderSizePixel = 0
CombatBtn.Text = ""
CombatBtn.AutoButtonColor = false
CombatBtn.Parent = Sidebar

local CombatCorner = Instance.new("UICorner")
CombatCorner.CornerRadius = UDim.new(0, 6)
CombatCorner.Parent = CombatBtn

local CombatText = Instance.new("TextLabel")
CombatText.Text = "Combat"
CombatText.Size = UDim2.new(1, 0, 1, 0)
CombatText.BackgroundTransparency = 1
CombatText.TextColor3 = Colors.TextDim
CombatText.Font = FONT_BOLD
CombatText.TextSize = 13
CombatText.Parent = CombatBtn

local ContentArea = Instance.new("Frame")
ContentArea.Size = UDim2.new(1, -150, 1, -50)
ContentArea.Position = UDim2.new(0, 145, 0, 45)
ContentArea.BackgroundTransparency = 1
ContentArea.Parent = MainFrame

local ContentScroll = Instance.new("ScrollingFrame")
ContentScroll.Size = UDim2.new(1, 0, 1, 0)
ContentScroll.BackgroundTransparency = 1
ContentScroll.BorderSizePixel = 0
ContentScroll.ScrollBarThickness = 3
ContentScroll.ScrollBarImageColor3 = Colors.Accent
ContentScroll.CanvasSize = UDim2.new(0, 0, 0, 1000)
ContentScroll.Parent = ContentArea

local CombatScroll = Instance.new("ScrollingFrame")
CombatScroll.Size = UDim2.new(1, 0, 1, 0)
CombatScroll.BackgroundTransparency = 1
CombatScroll.BorderSizePixel = 0
CombatScroll.ScrollBarThickness = 3
CombatScroll.ScrollBarImageColor3 = Colors.Accent
CombatScroll.CanvasSize = UDim2.new(0, 0, 0, 400)
CombatScroll.Visible = false
CombatScroll.Parent = ContentArea

local function showVisual()
    ContentScroll.Visible = true
    CombatScroll.Visible = false
    VisualBtn.BackgroundColor3 = Colors.Element
    VisualText.TextColor3 = Colors.Text
    CombatBtn.BackgroundColor3 = Colors.Sidebar
    CombatText.TextColor3 = Colors.TextDim
end

local function showCombat()
    ContentScroll.Visible = false
    CombatScroll.Visible = true
    VisualBtn.BackgroundColor3 = Colors.Sidebar
    VisualText.TextColor3 = Colors.TextDim
    CombatBtn.BackgroundColor3 = Colors.Element
    CombatText.TextColor3 = Colors.Text
end

VisualBtn.MouseButton1Click:Connect(showVisual)
CombatBtn.MouseButton1Click:Connect(showCombat)

-- ============================================
-- KEYBIND LIST
-- ============================================
local KeybindListFrame = Instance.new("Frame")
KeybindListFrame.Name = "KeybindList"
KeybindListFrame.Size = UDim2.new(0, 220, 0, 60)
KeybindListFrame.Position = UDim2.new(0, getgenv().KeybindListPosX or 20, 0, getgenv().KeybindListPosY or 100)
KeybindListFrame.BackgroundColor3 = Color3.fromRGB(12, 12, 16)
KeybindListFrame.BackgroundTransparency = 0.1
KeybindListFrame.BorderSizePixel = 0
KeybindListFrame.Visible = false
KeybindListFrame.Active = true
KeybindListFrame.Draggable = true
KeybindListFrame.ZIndex = 4000
KeybindListFrame.Parent = ScreenGui

local KbCorner = Instance.new("UICorner")
KbCorner.CornerRadius = UDim.new(0, 8)
KbCorner.Parent = KeybindListFrame

local KbGradient = Instance.new("UIGradient")
KbGradient.Color = ColorSequence.new({
    ColorSequenceKeypoint.new(0, Color3.fromRGB(15, 15, 25)),
    ColorSequenceKeypoint.new(0.5, Color3.fromRGB(25, 30, 50)),
    ColorSequenceKeypoint.new(1, Color3.fromRGB(15, 15, 25)),
})
KbGradient.Rotation = 90
KbGradient.Parent = KeybindListFrame

local KbStroke = Instance.new("UIStroke")
KbStroke.Thickness = 1.5
KbStroke.Color = Colors.Accent
KbStroke.Transparency = 0.3
KbStroke.Parent = KeybindListFrame

local KbTitle = Instance.new("TextLabel")
KbTitle.Text = "KEYBINDS"
KbTitle.Size = UDim2.new(1, 0, 0, 25)
KbTitle.Position = UDim2.new(0, 0, 0, 5)
KbTitle.BackgroundTransparency = 1
KbTitle.TextColor3 = Colors.Accent
KbTitle.Font = FONT_BOLD
KbTitle.TextSize = 13
KbTitle.TextXAlignment = Enum.TextXAlignment.Center
KbTitle.Parent = KeybindListFrame

local KbDivider = Instance.new("Frame")
KbDivider.Size = UDim2.new(1, -20, 0, 1)
KbDivider.Position = UDim2.new(0, 10, 0, 30)
KbDivider.BackgroundColor3 = Colors.Accent
KbDivider.BackgroundTransparency = 0.6
KbDivider.BorderSizePixel = 0
KbDivider.Parent = KeybindListFrame

local KbList = Instance.new("Frame")
KbList.Size = UDim2.new(1, -20, 0, 0)
KbList.Position = UDim2.new(0, 10, 0, 35)
KbList.BackgroundTransparency = 1
KbList.Parent = KeybindListFrame

local kbRows = {}

local function rebuildKeybindList()
    for _, row in ipairs(kbRows) do
        if row and row.Parent then row:Destroy() end
    end
    kbRows = {}

    local entries = {}

    if getgenv().TPKillerKeybind then
        table.insert(entries, {name = "TP KILLER", key = getgenv().TPKillerKeybind})
    end
    if getgenv().WalkSpeedKeybind then
        table.insert(entries, {name = "WALKSPEED", key = getgenv().WalkSpeedKeybind})
    end
    if getgenv().NoclipKeybind then
        table.insert(entries, {name = "NOCLIP", key = getgenv().NoclipKeybind})
    end

    if #entries == 0 then
        local emptyLabel = Instance.new("TextLabel")
        emptyLabel.Size = UDim2.new(1, 0, 0, 20)
        emptyLabel.Position = UDim2.new(0, 0, 0, 0)
        emptyLabel.BackgroundTransparency = 1
        emptyLabel.Text = "No keybinds"
        emptyLabel.TextColor3 = Color3.fromRGB(120, 120, 140)
        emptyLabel.Font = FONT
        emptyLabel.TextSize = 12
        emptyLabel.TextXAlignment = Enum.TextXAlignment.Left
        emptyLabel.Parent = KbList
        table.insert(kbRows, emptyLabel)
        KbList.Size = UDim2.new(1, -20, 0, 20)
        KeybindListFrame.Size = UDim2.new(0, 220, 0, 60)
        return
    end

    for i, entry in ipairs(entries) do
        local row = Instance.new("Frame")
        row.Size = UDim2.new(1, 0, 0, 20)
        row.Position = UDim2.new(0, 0, 0, (i - 1) * 22)
        row.BackgroundTransparency = 1
        row.Parent = KbList

        local nameLbl = Instance.new("TextLabel")
        nameLbl.Text = entry.name
        nameLbl.Size = UDim2.new(0.6, 0, 1, 0)
        nameLbl.Position = UDim2.new(0, 0, 0, 0)
        nameLbl.BackgroundTransparency = 1
        nameLbl.TextColor3 = Colors.Accent
        nameLbl.Font = FONT_BOLD
        nameLbl.TextSize = 12
        nameLbl.TextXAlignment = Enum.TextXAlignment.Left
        nameLbl.Parent = row

        local keyLbl = Instance.new("TextLabel")
        keyLbl.Text = "[" .. entry.key.Name .. "]"
        keyLbl.Size = UDim2.new(0.4, 0, 1, 0)
        keyLbl.Position = UDim2.new(0.6, 0, 0, 0)
        keyLbl.BackgroundTransparency = 1
        keyLbl.TextColor3 = Colors.Accent
        keyLbl.Font = FONT_BOLD
        keyLbl.TextSize = 12
        keyLbl.TextXAlignment = Enum.TextXAlignment.Right
        keyLbl.Parent = row

        table.insert(kbRows, row)
    end

    local totalHeight = #entries * 22
    KbList.Size = UDim2.new(1, -20, 0, totalHeight)
    KeybindListFrame.Size = UDim2.new(0, 220, 0, 60 + totalHeight)
end

task.spawn(function()
    while KeybindListFrame do
        if getgenv().KeybindListEnabled then
            rebuildKeybindList()
        end
        task.wait(0.3)
    end
end)

KeybindListFrame:GetPropertyChangedSignal("Position"):Connect(function()
    getgenv().KeybindListPosX = KeybindListFrame.Position.X.Offset
    getgenv().KeybindListPosY = KeybindListFrame.Position.Y.Offset
    saveConfig()
end)

-- ============================================
-- РЕГИСТРИРОВАНИЕ UI
-- ============================================
local ToggleRegistry = {}
local SliderRegistry = {}
local KeybindUIRegistry = {}

local function createToggle(parent, name, hintText, yPos, settingKey, callback)
    local ToggleContainer = Instance.new("Frame")
    ToggleContainer.Size = UDim2.new(1, -10, 0, 35)
    ToggleContainer.Position = UDim2.new(0, 0, 0, yPos)
    ToggleContainer.BackgroundColor3 = Colors.Element
    ToggleContainer.BorderSizePixel = 0
    ToggleContainer.Parent = parent

    local ToggleCorner = Instance.new("UICorner")
    ToggleCorner.CornerRadius = UDim.new(0, 6)
    ToggleCorner.Parent = ToggleContainer

    local ToggleLabel = Instance.new("TextLabel")
    ToggleLabel.Size = UDim2.new(1, -60, 1, 0)
    ToggleLabel.Position = UDim2.new(0, 15, 0, 0)
    ToggleLabel.BackgroundTransparency = 1
    ToggleLabel.TextColor3 = Colors.Text
    ToggleLabel.Font = FONT
    ToggleLabel.TextSize = 13
    ToggleLabel.TextXAlignment = Enum.TextXAlignment.Left
    ToggleLabel.RichText = true
    ToggleLabel.Parent = ToggleContainer

    if hintText and hintText ~= "" then
        ToggleLabel.Text = name .. ' <font color="rgb(130,130,160)">(' .. hintText .. ')</font>'
    else
        ToggleLabel.Text = name
    end

    local ToggleBtn = Instance.new("TextButton")
    ToggleBtn.Size = UDim2.new(0, 35, 0, 18)
    ToggleBtn.Position = UDim2.new(1, -45, 0.5, -9)
    ToggleBtn.BackgroundColor3 = Colors.ToggleOff
    ToggleBtn.Text = ""
    ToggleBtn.AutoButtonColor = false
    ToggleBtn.Parent = ToggleContainer

    local ToggleBtnCorner = Instance.new("UICorner")
    ToggleBtnCorner.CornerRadius = UDim.new(1, 0)
    ToggleBtnCorner.Parent = ToggleBtn

    local Circle = Instance.new("Frame")
    Circle.Size = UDim2.new(0, 14, 0, 14)
    Circle.Position = UDim2.new(0, 2, 0.5, -7)
    Circle.BackgroundColor3 = Colors.Text
    Circle.Parent = ToggleBtn

    local CircleCorner = Instance.new("UICorner")
    CircleCorner.CornerRadius = UDim.new(1, 0)
    CircleCorner.Parent = Circle

    local currentState = getgenv()[settingKey] == true

    local function updateVisual(state)
        if state then
            TweenService:Create(ToggleBtn, TweenInfo.new(0.2), {BackgroundColor3 = Colors.ToggleOn}):Play()
            TweenService:Create(Circle, TweenInfo.new(0.2), {Position = UDim2.new(1, -16, 0.5, -7)}):Play()
        else
            TweenService:Create(ToggleBtn, TweenInfo.new(0.2), {BackgroundColor3 = Colors.ToggleOff}):Play()
            TweenService:Create(Circle, TweenInfo.new(0.2), {Position = UDim2.new(0, 2, 0.5, -7)}):Play()
        end
    end

    updateVisual(currentState)
    ToggleRegistry[settingKey] = function() updateVisual(getgenv()[settingKey] == true) end

    ToggleBtn.MouseButton1Click:Connect(function()
        currentState = not currentState
        getgenv()[settingKey] = currentState
        updateVisual(currentState)
        saveConfig()
        local ok2, err2 = pcall(callback, currentState)
        if not ok2 then warn("[Toggle] Ошибка:", err2) end
    end)
end

local function createButtonWithKeybind(parent, name, yPos, bindKeyName, actionCallback, bindId)
    local BtnContainer = Instance.new("Frame")
    BtnContainer.Size = UDim2.new(1, -10, 0, 35)
    BtnContainer.Position = UDim2.new(0, 0, 0, yPos)
    BtnContainer.BackgroundColor3 = Colors.Button
    BtnContainer.BorderSizePixel = 0
    BtnContainer.Parent = parent

    local BtnCorner = Instance.new("UICorner")
    BtnCorner.CornerRadius = UDim.new(0, 6)
    BtnCorner.Parent = BtnContainer

    local BtnStroke = Instance.new("UIStroke")
    BtnStroke.Color = Colors.Accent
    BtnStroke.Thickness = 1
    BtnStroke.Transparency = 0.7
    BtnStroke.Parent = BtnContainer

    local Btn = Instance.new("TextButton")
    Btn.Size = UDim2.new(1, -45, 1, 0)
    Btn.BackgroundTransparency = 1
    Btn.Text = ""
    Btn.AutoButtonColor = false
    Btn.Parent = BtnContainer

    local BtnLabel = Instance.new("TextLabel")
    BtnLabel.Text = name
    BtnLabel.Size = UDim2.new(1, 0, 1, 0)
    BtnLabel.BackgroundTransparency = 1
    BtnLabel.TextColor3 = Colors.Text
    BtnLabel.Font = FONT_BOLD
    BtnLabel.TextSize = 13
    BtnLabel.Parent = Btn

    local KeybindBtn = Instance.new("TextButton")
    KeybindBtn.Size = UDim2.new(0, 40, 0, 28)
    KeybindBtn.Position = UDim2.new(1, -43, 0.5, -14)
    KeybindBtn.BackgroundColor3 = Colors.KeybindBG
    KeybindBtn.Text = ""
    KeybindBtn.AutoButtonColor = false
    KeybindBtn.Parent = BtnContainer

    local KeybindCorner = Instance.new("UICorner")
    KeybindCorner.CornerRadius = UDim.new(0, 5)
    KeybindCorner.Parent = KeybindBtn

    local KeybindLabel = Instance.new("TextLabel")
    KeybindLabel.Text = "•••"
    KeybindLabel.Size = UDim2.new(1, 0, 1, 0)
    KeybindLabel.BackgroundTransparency = 1
    KeybindLabel.TextColor3 = Colors.TextDim
    KeybindLabel.Font = FONT_BOLD
    KeybindLabel.TextSize = 14
    KeybindLabel.Parent = KeybindBtn

    local function updateKeybindVisual()
        if getgenv().ListeningForKeybind and getgenv().BindTarget == bindId then
            KeybindLabel.Text = "..."
            KeybindLabel.TextColor3 = Colors.KeybindListening
            KeybindBtn.BackgroundColor3 = Color3.fromRGB(70, 50, 20)
        elseif getgenv()[bindKeyName] then
            KeybindLabel.Text = keyCodeToString(getgenv()[bindKeyName])
            KeybindLabel.TextColor3 = Colors.Accent
            KeybindBtn.BackgroundColor3 = Colors.KeybindBG
        else
            KeybindLabel.Text = "•••"
            KeybindLabel.TextColor3 = Colors.TextDim
            KeybindBtn.BackgroundColor3 = Colors.KeybindBG
        end
    end

    KeybindUIRegistry[bindKeyName] = updateKeybindVisual
    updateKeybindVisual()

    Btn.MouseEnter:Connect(function()
        TweenService:Create(BtnContainer, TweenInfo.new(0.15), {BackgroundColor3 = Colors.ButtonHover}):Play()
        TweenService:Create(BtnStroke, TweenInfo.new(0.15), {Transparency = 0.2}):Play()
    end)

    Btn.MouseLeave:Connect(function()
        TweenService:Create(BtnContainer, TweenInfo.new(0.15), {BackgroundColor3 = Colors.Button}):Play()
        TweenService:Create(BtnStroke, TweenInfo.new(0.15), {Transparency = 0.7}):Play()
    end)

    Btn.MouseButton1Click:Connect(function()
        local ok2, err2 = pcall(actionCallback)
        if not ok2 then warn("[Button] Ошибка:", err2) end
        BtnContainer.BackgroundColor3 = Colors.Accent
        TweenService:Create(BtnContainer, TweenInfo.new(0.3), {BackgroundColor3 = Colors.Button}):Play()
    end)

    KeybindBtn.MouseButton1Click:Connect(function()
        getgenv().ListeningForKeybind = true
        getgenv().BindTarget = bindId
        updateKeybindVisual()
    end)

    task.spawn(function()
        while BtnContainer.Parent do
            updateKeybindVisual()
            task.wait(0.1)
        end
    end)
end

local function createSlider(parent, name, yPos, minVal, maxVal, settingKey, callback)
    local SliderContainer = Instance.new("Frame")
    SliderContainer.Size = UDim2.new(1, -10, 0, 45)
    SliderContainer.Position = UDim2.new(0, 0, 0, yPos)
    SliderContainer.BackgroundColor3 = Colors.Element
    SliderContainer.BorderSizePixel = 0
    SliderContainer.Parent = parent

    local SliderCorner = Instance.new("UICorner")
    SliderCorner.CornerRadius = UDim.new(0, 6)
    SliderCorner.Parent = SliderContainer

    local SliderLabel = Instance.new("TextLabel")
    SliderLabel.Text = name
    SliderLabel.Size = UDim2.new(1, -80, 0, 20)
    SliderLabel.Position = UDim2.new(0, 15, 0, 3)
    SliderLabel.BackgroundTransparency = 1
    SliderLabel.TextColor3 = Colors.Text
    SliderLabel.Font = FONT
    SliderLabel.TextSize = 13
    SliderLabel.TextXAlignment = Enum.TextXAlignment.Left
    SliderLabel.Parent = SliderContainer

    local SliderValue = Instance.new("TextLabel")
    SliderValue.Text = tostring(getgenv()[settingKey])
    SliderValue.Size = UDim2.new(0, 60, 0, 20)
    SliderValue.Position = UDim2.new(1, -70, 0, 3)
    SliderValue.BackgroundTransparency = 1
    SliderValue.TextColor3 = Colors.Accent
    SliderValue.Font = FONT_BOLD
    SliderValue.TextSize = 13
    SliderValue.TextXAlignment = Enum.TextXAlignment.Right
    SliderValue.Parent = SliderContainer

    local track = Instance.new("Frame")
    track.Size = UDim2.new(1, -30, 0, 6)
    track.Position = UDim2.new(0, 15, 0, 30)
    track.BackgroundColor3 = Color3.fromRGB(25, 25, 25)
    track.BorderSizePixel = 0
    track.Parent = SliderContainer

    local trackCorner = Instance.new("UICorner")
    trackCorner.CornerRadius = UDim.new(1, 0)
    trackCorner.Parent = track

    local currentVal = getgenv()[settingKey] or minVal
    local initRelX = math.clamp((currentVal - minVal) / (maxVal - minVal), 0, 1)

    local fill = Instance.new("Frame")
    fill.Size = UDim2.new(initRelX, 0, 1, 0)
    fill.BackgroundColor3 = Colors.Accent
    fill.BorderSizePixel = 0
    fill.Parent = track

    local fillCorner = Instance.new("UICorner")
    fillCorner.CornerRadius = UDim.new(1, 0)
    fillCorner.Parent = fill

    local knob = Instance.new("Frame")
    knob.Size = UDim2.new(0, 16, 0, 16)
    knob.Position = UDim2.new(initRelX, -8, 0.5, -8)
    knob.BackgroundColor3 = Color3.fromRGB(255, 255, 255)
    knob.BorderSizePixel = 0
    knob.Parent = track

    local knobCorner = Instance.new("UICorner")
    knobCorner.CornerRadius = UDim.new(1, 0)
    knobCorner.Parent = knob

    local hitbox = Instance.new("TextButton")
    hitbox.Size = UDim2.new(1, 0, 1, 0)
    hitbox.BackgroundTransparency = 1
    hitbox.Text = ""
    hitbox.Parent = track

    local dragging = false

    local function updateFromX(x)
        local relX = math.clamp((x - track.AbsolutePosition.X) / track.AbsoluteSize.X, 0, 1)
        local val = math.floor(minVal + (maxVal - minVal) * relX + 0.5)
        SliderValue.Text = tostring(val)
        fill.Size = UDim2.new(relX, 0, 1, 0)
        knob.Position = UDim2.new(relX, -8, 0.5, -8)
        getgenv()[settingKey] = val
        saveConfig()
        local ok2, err2 = pcall(callback, val)
        if not ok2 then warn("[Slider] Ошибка:", err2) end
    end

    SliderRegistry[settingKey] = function()
        local v = getgenv()[settingKey] or minVal
        local relX = math.clamp((v - minVal) / (maxVal - minVal), 0, 1)
        SliderValue.Text = tostring(v)
        fill.Size = UDim2.new(relX, 0, 1, 0)
        knob.Position = UDim2.new(relX, -8, 0.5, -8)
    end

    hitbox.InputBegan:Connect(function(input)
        if input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch then
            dragging = true
            updateFromX(input.Position.X)
        end
    end)

    UserInputService.InputChanged:Connect(function(input)
        if dragging and (input.UserInputType == Enum.UserInputType.MouseMovement or input.UserInputType == Enum.UserInputType.Touch) then
            updateFromX(input.Position.X)
        end
    end)

    UserInputService.InputEnded:Connect(function(input)
        if input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch then
            dragging = false
        end
    end)
end

-- VISUAL
createToggle(ContentScroll, "ESP", "включать сразу в катке", 0, "ESPEnabled", function(state)
    if state then startESP() else stopESP() end
end)

createToggle(ContentScroll, "ESP 2.0", "surv=зелёный / killer=красный", 45, "ESP2Enabled", function(state)
    if state then startESP2() else stopESP2() end
end)

createToggle(ContentScroll, "ESP Generator", "", 90, "GenESPEnabled", function(state)
    if state then startGenESP() else stopGenESP() end
end)

createToggle(ContentScroll, "ESP Pallet", "", 135, "PalletESPEnabled", function(state)
    if state then startPalletESP() else stopPalletESP() end
end)

createToggle(ContentScroll, "Fullbright", "", 180, "FullbrightEnabled", function(state)
    if state then applyFullbright() else restoreLighting() end
end)

createToggle(ContentScroll, "Blue Fog", "", 225, "BlueFogEnabled", function(state)
    if state then createBlueFog() else destroyBlueFog() end
end)

createToggle(ContentScroll, "Keybind List", "", 270, "KeybindListEnabled", function(state)
    KeybindListFrame.Visible = state
    if state then rebuildKeybindList() end
end)

createToggle(ContentScroll, "Lightborn", "не ослепляться фонариком", 315, "LightbornEnabled", function(state)
    setLightborn(state)
end)

createButtonWithKeybind(ContentScroll, "TP KILLER", 360, "TPKillerKeybind", function()
    teleportBehindKiller()
    showNotification("TP KILLER", "Teleported behind killer")
end, "tp")

createSlider(ContentScroll, "Brightness", 405, 0, 10, "FullbrightValue", function(val)
    if getgenv().FullbrightEnabled then applyFullbright() end
end)

createSlider(ContentScroll, "FOV", 460, 70, 120, "FOVValue", function(val) end)

-- COMBAT
createButtonWithKeybind(CombatScroll, "WalkSpeed", 0, "WalkSpeedKeybind", function()
    getgenv().WalkSpeedActive = not getgenv().WalkSpeedActive
    saveConfig()
    showNotification("WALKSPEED", getgenv().WalkSpeedActive and "Activated" or "Deactivated")
end, "walkspeed")

createSlider(CombatScroll, "Speed", 45, 0, 200, "WalkSpeedValue", function(val) end)

createToggle(CombatScroll, "Jump", "", 100, "JumpEnabled", function(state)
    local char = player.Character
    if char then
        local hum = char:FindFirstChildOfClass("Humanoid")
        if hum then
            hum:SetStateEnabled(Enum.HumanoidStateType.Jumping, state)
        end
    end
end)

createSlider(CombatScroll, "JumpPower", 145, 0, 500, "JumpPowerValue", function(val) end)

createButtonWithKeybind(CombatScroll, "Noclip", 200, "NoclipKeybind", function()
    setNoclip(not getgenv().NoclipEnabled)
    saveConfig()
    showNotification("NOCLIP", getgenv().NoclipEnabled and "Activated" or "Deactivated")
end, "noclip")

-----------------------------------------------------------
-- // АВТОПРИМЕНЕНИЕ ПРИ СТАРТЕ
-----------------------------------------------------------
task.spawn(function()
    task.wait(1)

    if getgenv().ESPEnabled then startESP() end
    if getgenv().ESP2Enabled then startESP2() end
    if getgenv().GenESPEnabled then startGenESP() end
    if getgenv().PalletESPEnabled then startPalletESP() end
    if getgenv().FullbrightEnabled then applyFullbright() end
    if getgenv().BlueFogEnabled then createBlueFog() end
    if getgenv().NoclipEnabled then enableNoclip() end
    if getgenv().LightbornEnabled then enableLightborn() end
    if getgenv().KeybindListEnabled then
        KeybindListFrame.Visible = true
        rebuildKeybindList()
    end

    for key, updater in pairs(ToggleRegistry) do pcall(updater) end
    for key, updater in pairs(SliderRegistry) do pcall(updater) end
    for key, updater in pairs(KeybindUIRegistry) do pcall(updater) end

    Camera.FieldOfView = getgenv().FOVValue or 70
end)

-----------------------------------------------------------
-- // ПРОФИЛЬ
-----------------------------------------------------------
local UserProfile = Instance.new("Frame")
UserProfile.Size = UDim2.new(0, 120, 0, 50)
UserProfile.Position = UDim2.new(0, 10, 1, -60)
UserProfile.BackgroundColor3 = Colors.Sidebar
UserProfile.BorderSizePixel = 0
UserProfile.Parent = MainFrame

local Avatar = Instance.new("ImageLabel")
Avatar.Size = UDim2.new(0, 35, 0, 35)
Avatar.Position = UDim2.new(0, 5, 0.5, -17)
Avatar.BackgroundColor3 = Colors.Element
Avatar.Image = "rbxthumb://type=AvatarHeadShot&id=" .. player.UserId .. "&w=150&h=150"
Avatar.Parent = UserProfile

local AvatarCorner = Instance.new("UICorner")
AvatarCorner.CornerRadius = UDim.new(0, 6)
AvatarCorner.Parent = Avatar

local NameLabel = Instance.new("TextLabel")
NameLabel.Text = string.sub(player.Name, 1, 10) .. "..."
NameLabel.Size = UDim2.new(1, -50, 0, 20)
NameLabel.Position = UDim2.new(0, 45, 0, 8)
NameLabel.BackgroundTransparency = 1
NameLabel.TextColor3 = Colors.Text
NameLabel.Font = FONT_BOLD
NameLabel.TextSize = 12
NameLabel.TextXAlignment = Enum.TextXAlignment.Left
NameLabel.Parent = UserProfile

local RoleLabel = Instance.new("TextLabel")
RoleLabel.Text = "IO3ep"
RoleLabel.Size = UDim2.new(1, -50, 0, 15)
RoleLabel.Position = UDim2.new(0, 45, 0, 25)
RoleLabel.BackgroundTransparency = 1
RoleLabel.TextColor3 = Color3.fromRGB(100, 200, 100)
RoleLabel.Font = FONT
RoleLabel.TextSize = 11
RoleLabel.TextXAlignment = Enum.TextXAlignment.Left
RoleLabel.Parent = UserProfile

-----------------------------------------------------------
-- // RIGHT SHIFT
-----------------------------------------------------------
local menuOpen = true

local function setMenuVisible(state)
    menuOpen = state
    if state then
        MainFrame.Visible = true
        MainFrame.Size = UDim2.new(0, 0, 0, 0)
        MainFrame.Position = UDim2.new(0.5, 0, 0.5, 0)
        TweenService:Create(MainFrame, TweenInfo.new(0.25, Enum.EasingStyle.Back, Enum.EasingDirection.Out), {
            Size = UDim2.new(0, 600, 0, 400),
            Position = UDim2.new(0.5, -300, 0.5, -200),
        }):Play()
    else
        local t = TweenService:Create(MainFrame, TweenInfo.new(0.2, Enum.EasingStyle.Quad, Enum.EasingDirection.In), {
            Size = UDim2.new(0, 0, 0, 0),
            Position = UDim2.new(0.5, 0, 0.5, 0),
        })
        t:Play()
        t.Completed:Connect(function()
            if not menuOpen then MainFrame.Visible = false end
        end)
    end
end

UserInputService.InputBegan:Connect(function(input, gp)
    if gp then return end
    if input.KeyCode == Enum.KeyCode.RightShift then
        setMenuVisible(not menuOpen)
    end
end)

print("[delux.cc] Loaded successfully.")
