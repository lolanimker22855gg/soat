local cloneref = cloneref or function(o)
    return o
end

local function Service(name)
    return cloneref(game:GetService(name))
end

local Players = Service("Players")
local UserInputService = Service("UserInputService")
local TweenService = Service("TweenService")
local RunService = Service("RunService")
local HttpService = Service("HttpService")
local GuiService = Service("GuiService")
local LocalPlayer = Players.LocalPlayer

local Env = getgenv and getgenv() or _G
if Env.UILibrary and Env.UILibrary.Unload then
    pcall(function()
        Env.UILibrary:Unload()
    end)
end

local rgb = Color3.fromRGB
local floor, clamp, min, max = math.floor, math.clamp, math.min, math.max

local Library = {
    Flags = {},
    Options = {},
    Windows = {},
    Registry = {},
    Connections = {},
    Animated = {},
    Keybinds = {},
    HotkeyEntries = {},
    Folder = "UILibrary",
    Unloaded = false,
    Popup = nil,
    Listening = nil,
    OnUnload = {},
    Theme = {
        Background = rgb(9, 9, 9),
        Panel = rgb(16, 16, 16),
        Element = rgb(25, 25, 25),
        Hover = rgb(36, 36, 36),
        Border = rgb(48, 48, 48),
        Text = rgb(240, 240, 240),
        SubText = rgb(128, 128, 128),
        Accent = rgb(255, 255, 255),
        OnAccent = rgb(10, 10, 10),
    },
    Icons = {
        target = "◎",
        crosshair = "◎",
        shield = "◈",
        eye = "◉",
        globe = "◍",
        user = "☻",
        users = "☻",
        bolt = "ϟ",
        gear = "⚙",
        settings = "⚙",
        file = "▤",
        search = "⌕",
        bomb = "✸",
        key = "⌨",
        keyboard = "⌨",
        robot = "▣",
        wrench = "✎",
        list = "☰",
        home = "⌂",
        star = "★",
        heart = "♥",
        flower = "✿",
        dot = "●",
        square = "■",
        diamond = "◆",
        triangle = "▲",
        arrow = "➤",
        move = "✥",
        palette = "◐",
        info = "ℹ",
        folder = "▣",
        save = "▼",
    },
}

local FontRegular = Font.fromEnum(Enum.Font.GothamMedium)
local FontBold = Font.fromEnum(Enum.Font.GothamBold)

local Defaults = {
    Frame = { BorderSizePixel = 0 },
    CanvasGroup = { BorderSizePixel = 0 },
    ScrollingFrame = {
        BorderSizePixel = 0,
        BackgroundTransparency = 1,
        ScrollBarThickness = 2,
        ScrollBarImageColor3 = rgb(80, 80, 80),
        CanvasSize = UDim2.new(),
        AutomaticCanvasSize = Enum.AutomaticSize.Y,
        ScrollingDirection = Enum.ScrollingDirection.Y,
        ElasticBehavior = Enum.ElasticBehavior.Never,
    },
    TextLabel = {
        BorderSizePixel = 0,
        BackgroundTransparency = 1,
        FontFace = FontRegular,
        TextSize = 12,
        TextXAlignment = Enum.TextXAlignment.Left,
    },
    TextButton = {
        BorderSizePixel = 0,
        BackgroundTransparency = 1,
        FontFace = FontRegular,
        TextSize = 12,
        AutoButtonColor = false,
        Text = "",
    },
    TextBox = {
        BorderSizePixel = 0,
        BackgroundTransparency = 1,
        FontFace = FontRegular,
        TextSize = 12,
        ClearTextOnFocus = false,
        TextXAlignment = Enum.TextXAlignment.Left,
    },
    ImageLabel = { BorderSizePixel = 0, BackgroundTransparency = 1 },
    UIListLayout = { SortOrder = Enum.SortOrder.LayoutOrder },
}

function Library:Reg(inst, prop, key)
    inst[prop] = self.Theme[key]
    local list = self.Registry[key]
    if not list then
        list = {}
        self.Registry[key] = list
    end
    list[#list + 1] = { inst, prop }
end

local function Create(class, props, children)
    local inst = Instance.new(class)
    local defaults = Defaults[class]
    if defaults then
        for k, v in pairs(defaults) do
            inst[k] = v
        end
    end
    local theme, parent
    if props then
        for k, v in pairs(props) do
            if k == "Theme" then
                theme = v
            elseif k == "Parent" then
                parent = v
            else
                inst[k] = v
            end
        end
    end
    if theme then
        for k, v in pairs(theme) do
            Library:Reg(inst, k, v)
        end
    end
    if children then
        for _, child in ipairs(children) do
            child.Parent = inst
        end
    end
    if parent then
        inst.Parent = parent
    end
    return inst
end

local function Corner(parent, radius)
    return Create("UICorner", { CornerRadius = UDim.new(0, radius), Parent = parent })
end

local function Stroke(parent, key, thickness)
    return Create("UIStroke", {
        Thickness = thickness or 1,
        ApplyStrokeMode = Enum.ApplyStrokeMode.Border,
        Theme = { Color = key or "Border" },
        Parent = parent,
    })
end

local function Pad(parent, l, t, r, b)
    return Create("UIPadding", {
        PaddingLeft = UDim.new(0, l),
        PaddingTop = UDim.new(0, t or l),
        PaddingRight = UDim.new(0, r or l),
        PaddingBottom = UDim.new(0, b or t or l),
        Parent = parent,
    })
end

local function List(parent, padding, horizontal, align)
    return Create("UIListLayout", {
        Padding = UDim.new(0, padding or 0),
        FillDirection = horizontal and Enum.FillDirection.Horizontal or Enum.FillDirection.Vertical,
        HorizontalAlignment = align or Enum.HorizontalAlignment.Left,
        VerticalAlignment = horizontal and Enum.VerticalAlignment.Center or Enum.VerticalAlignment.Top,
        Parent = parent,
    })
end

local function Tween(inst, props, time)
    local tween = TweenService:Create(inst, TweenInfo.new(time or 0.15, Enum.EasingStyle.Quad, Enum.EasingDirection.Out), props)
    tween:Play()
    return tween
end

local function Inside(gui, point)
    local p, s = gui.AbsolutePosition, gui.AbsoluteSize
    return point.X >= p.X and point.X <= p.X + s.X and point.Y >= p.Y and point.Y <= p.Y + s.Y
end

local function Pointer(input)
    if input and input.UserInputType == Enum.UserInputType.Touch then
        return Vector2.new(input.Position.X, input.Position.Y + GuiService:GetGuiInset().Y)
    end
    return UserInputService:GetMouseLocation()
end

local function Track(inst, onMove, onEnd)
    inst.InputBegan:Connect(function(input)
        if input.UserInputType ~= Enum.UserInputType.MouseButton1 and input.UserInputType ~= Enum.UserInputType.Touch then
            return
        end
        local start = Pointer(input)
        local moveConn, endConn
        local function stop()
            if moveConn then
                moveConn:Disconnect()
            end
            if endConn then
                endConn:Disconnect()
            end
            if onEnd then
                onEnd()
            end
        end
        onMove(start, start)
        moveConn = UserInputService.InputChanged:Connect(function(changed)
            if changed.UserInputType == Enum.UserInputType.MouseMovement or changed.UserInputType == Enum.UserInputType.Touch then
                onMove(Pointer(changed), start)
            end
        end)
        endConn = UserInputService.InputEnded:Connect(function(ended)
            if ended == input or ended.UserInputType == Enum.UserInputType.MouseButton1 then
                stop()
            end
        end)
    end)
end

local function MakeDraggable(handle, target)
    local origin
    Track(handle, function(pt, start)
        if not origin then
            origin = target.AbsolutePosition
        end
        target.Position = UDim2.fromOffset(origin.X + pt.X - start.X, origin.Y + pt.Y - start.Y)
    end, function()
        origin = nil
    end)
end

local function MakeIcon(parent, icon, size, themeKey, props)
    icon = Library.Icons[icon] or icon
    props = props or {}
    local inst
    if type(icon) == "number" or (type(icon) == "string" and icon:find("rbxasset")) then
        inst = Create("ImageLabel", {
            Image = type(icon) == "number" and ("rbxassetid://" .. icon) or icon,
            Size = UDim2.fromOffset(size, size),
            Theme = { ImageColor3 = themeKey or "SubText" },
        })
    else
        inst = Create("TextLabel", {
            Text = icon or "",
            TextSize = size,
            TextXAlignment = Enum.TextXAlignment.Center,
            Size = UDim2.fromOffset(size + 4, size + 4),
            Theme = { TextColor3 = themeKey or "SubText" },
        })
    end
    for k, v in pairs(props) do
        inst[k] = v
    end
    inst.Parent = parent
    return inst
end

local KeyShort = {
    LeftShift = "LShift",
    RightShift = "RShift",
    LeftControl = "LCtrl",
    RightControl = "RCtrl",
    LeftAlt = "LAlt",
    RightAlt = "RAlt",
    Return = "Enter",
    Backspace = "Bksp",
    Escape = "Esc",
    MouseButton1 = "M1",
    MouseButton2 = "M2",
    MouseButton3 = "M3",
    Insert = "Ins",
    Delete = "Del",
    PageUp = "PgUp",
    PageDown = "PgDn",
    CapsLock = "Caps",
    One = "1",
    Two = "2",
    Three = "3",
    Four = "4",
    Five = "5",
    Six = "6",
    Seven = "7",
    Eight = "8",
    Nine = "9",
    Zero = "0",
}

local DigitKeys = {}
for name, digit in pairs(KeyShort) do
    if digit:match("^%d$") then
        DigitKeys[digit] = name
    end
end

local function KeyName(key)
    if not key then
        return "None"
    end
    return KeyShort[key.Name] or key.Name
end

local function ParseKey(value)
    if value == nil or value == false then
        return nil
    end
    if typeof(value) == "EnumItem" then
        return value
    end
    if type(value) == "string" then
        local up = value:upper()
        local n = up:match("^MB(%d)$") or up:match("^MOUSEBUTTON(%d)$") or up:match("^M(%d)$")
        if n and tonumber(n) <= 3 and tonumber(n) >= 1 then
            return Enum.UserInputType["MouseButton" .. n]
        end
        if DigitKeys[value] then
            return Enum.KeyCode[DigitKeys[value]]
        end
        local ok, result = pcall(function()
            return Enum.KeyCode[value]
        end)
        if ok and result then
            return result
        end
        ok, result = pcall(function()
            return Enum.KeyCode[up]
        end)
        if ok and result then
            return result
        end
    end
    return nil
end

local function MatchKey(input, key)
    if key.EnumType == Enum.KeyCode then
        return input.KeyCode == key
    end
    return input.UserInputType == key
end

local function Class()
    local class = {}
    class.__index = class
    return class
end

local Control = Class()

function Control:Get()
    return self.Value
end

function Control:OnChanged(fn)
    self.Callbacks[#self.Callbacks + 1] = fn
    return self
end

function Control:Fire(...)
    for _, fn in ipairs(self.Callbacks) do
        local ok, err = pcall(fn, ...)
        if not ok then
            warn("[UILibrary] " .. tostring(err))
        end
    end
end

function Control:SetVisible(state)
    if self.Entry then
        self.Entry.Visible = state and true or false
        self.Section.Tab.Window:_Filter()
    end
end

local function Subclass()
    local class = {}
    class.__index = class
    return setmetatable(class, { __index = Control })
end

local Toggle = Subclass()
local Slider = Subclass()
local RangeSlider = Subclass()
local Dropdown = Subclass()
local ListBox = Subclass()
local ButtonControl = Subclass()
local Input = Subclass()
local Keybind = Subclass()
local ColorPicker = Subclass()
local LabelControl = Subclass()

local Section = {}
Section.__index = Section

local Tab = {}
Tab.__index = Tab

local Window = {}
Window.__index = Window

function Library:Connect(signal, fn)
    local conn = signal:Connect(fn)
    self.Connections[#self.Connections + 1] = conn
    return conn
end

function Library:SetTheme(theme)
    for key, value in pairs(theme) do
        self.Theme[key] = value
        local list = self.Registry[key]
        if list then
            for i = #list, 1, -1 do
                local entry = list[i]
                if entry[1].Parent then
                    entry[1][entry[2]] = value
                else
                    table.remove(list, i)
                end
            end
        end
    end
    for _, option in pairs(self.Options) do
        if option._Visual then
            option:_Visual(true)
        end
    end
end

function Library:SetAccent(color)
    local lum = 0.299 * color.R + 0.587 * color.G + 0.114 * color.B
    self:SetTheme({ Accent = color, OnAccent = lum > 0.55 and rgb(10, 10, 10) or rgb(245, 245, 245) })
end

local function GetGuiParent()
    local ok, hui = pcall(function()
        return gethui and gethui()
    end)
    if ok and hui then
        return hui
    end
    local core = cloneref(game:GetService("CoreGui"))
    local test = Instance.new("Folder")
    local good = pcall(function()
        test.Parent = core
    end)
    test:Destroy()
    if good then
        return core
    end
    return LocalPlayer:WaitForChild("PlayerGui")
end

function Library:_Init()
    if self.Root then
        return
    end
    local root = Instance.new("ScreenGui")
    root.Name = HttpService:GenerateGUID(false):sub(1, 12)
    root.ResetOnSpawn = false
    root.IgnoreGuiInset = true
    root.ZIndexBehavior = Enum.ZIndexBehavior.Sibling
    root.DisplayOrder = 10000
    if syn and syn.protect_gui then
        pcall(syn.protect_gui, root)
    end
    root.Parent = GetGuiParent()
    self.Root = root
    self.Popups = Create("Frame", {
        Name = "Popups",
        BackgroundTransparency = 1,
        Size = UDim2.fromScale(1, 1),
        ZIndex = 100,
        Parent = root,
    })
    self.NotifHolder = Create("Frame", {
        Name = "Notifications",
        BackgroundTransparency = 1,
        AnchorPoint = Vector2.new(1, 1),
        Position = UDim2.new(1, -16, 1, -16),
        Size = UDim2.fromOffset(260, 600),
        ZIndex = 50,
        Parent = root,
    })
    Create("UIListLayout", {
        Padding = UDim.new(0, 8),
        VerticalAlignment = Enum.VerticalAlignment.Bottom,
        HorizontalAlignment = Enum.HorizontalAlignment.Right,
        Parent = self.NotifHolder,
    })

    self:Connect(UserInputService.InputBegan, function(input, processed)
        local listening = self.Listening
        if listening then
            listening:_Capture(input)
            return
        end
        local pressType = input.UserInputType
        if pressType == Enum.UserInputType.MouseButton1 or pressType == Enum.UserInputType.Touch then
            local popup = self.Popup
            if popup then
                local pt = Pointer(input)
                if not Inside(popup.Frame, pt) and not (popup.Owner and Inside(popup.Owner, pt)) then
                    self:ClosePopup()
                end
            end
        end
        if UserInputService:GetFocusedTextBox() then
            return
        end
        for _, window in ipairs(self.Windows) do
            if window.MenuKey and MatchKey(input, window.MenuKey) then
                window:Toggle()
            end
        end
        for kb in pairs(self.Keybinds) do
            if kb.Key and not kb.NoTrigger and MatchKey(input, kb.Key) then
                if kb.Mode == "Hold" then
                    kb:SetState(true)
                else
                    local current = kb.Toggle and kb.Toggle.Value or kb.State
                    kb:SetState(not current)
                end
            end
        end
    end)

    self:Connect(UserInputService.InputEnded, function(input)
        for kb in pairs(self.Keybinds) do
            if kb.Mode == "Hold" and kb.Key and not kb.NoTrigger and MatchKey(input, kb.Key) then
                kb:SetState(false)
            end
        end
    end)

    local frames, accum = 0, 0
    self.FPS = 60
    self:Connect(RunService.RenderStepped, function(dt)
        frames = frames + 1
        accum = accum + dt
        if accum >= 0.5 then
            self.FPS = floor(frames / accum + 0.5)
            frames, accum = 0, 0
            self:_UpdateWatermark()
        end
        local t = os.clock()
        for cp in pairs(self.Animated) do
            if cp.Animation == "Rainbow" then
                cp.H, cp.S, cp.V = (t * 0.2) % 1, 1, 1
            elseif cp.Animation == "Pulse" then
                cp.H, cp.S = cp.Base[1], cp.Base[2]
                cp.V = clamp(cp.Base[3] * (0.35 + 0.65 * (0.5 + 0.5 * math.sin(t * 3))), 0, 1)
            end
            cp:_Update(true)
        end
    end)
end

function Library:OpenPopup(frame, owner, onClose)
    self:ClosePopup()
    frame.Visible = true
    self.Popup = { Frame = frame, Owner = owner, OnClose = onClose }
end

function Library:ClosePopup()
    local popup = self.Popup
    if popup then
        self.Popup = nil
        popup.Frame.Visible = false
        if popup.OnClose then
            popup.OnClose()
        end
    end
end

function Library:IsPopupOpen(frame)
    return self.Popup ~= nil and self.Popup.Frame == frame
end

function Library:Place(frame, owner, alignRight)
    local pos, size = owner.AbsolutePosition, owner.AbsoluteSize
    local w, h = frame.Size.X.Offset, frame.Size.Y.Offset
    local view = self.Root.AbsoluteSize
    local x = alignRight and (pos.X + size.X - w) or pos.X
    local y = pos.Y + size.Y + 4
    if y + h > view.Y - 4 then
        y = pos.Y - h - 4
    end
    x = clamp(x, 4, max(4, view.X - w - 4))
    y = clamp(y, 4, max(4, view.Y - h - 4))
    frame.Position = UDim2.fromOffset(x, y)
end

function Library:Notify(opts, duration)
    self:_Init()
    if type(opts) ~= "table" then
        opts = { Text = tostring(opts), Duration = duration }
    end
    local time = opts.Duration or duration or 4
    self.NotifOrder = (self.NotifOrder or 0) + 1
    local card = Create("CanvasGroup", {
        Size = UDim2.fromOffset(250, 0),
        AutomaticSize = Enum.AutomaticSize.Y,
        GroupTransparency = 1,
        LayoutOrder = self.NotifOrder,
        Theme = { BackgroundColor3 = "Panel" },
        Parent = self.NotifHolder,
    })
    Corner(card, 6)
    Stroke(card)
    Pad(card, 10, 8, 10, 0)
    List(card, 4)
    if opts.Title then
        Create("TextLabel", {
            Size = UDim2.new(1, 0, 0, 14),
            Text = opts.Title,
            FontFace = FontBold,
            TextSize = 12,
            LayoutOrder = 1,
            Theme = { TextColor3 = "Text" },
            Parent = card,
        })
    end
    Create("TextLabel", {
        Size = UDim2.new(1, 0, 0, 0),
        AutomaticSize = Enum.AutomaticSize.Y,
        Text = opts.Text or "",
        TextWrapped = true,
        TextSize = 12,
        LayoutOrder = 2,
        Theme = { TextColor3 = opts.Title and "SubText" or "Text" },
        Parent = card,
    })
    local barHolder = Create("Frame", {
        Size = UDim2.new(1, 20, 0, 6),
        BackgroundTransparency = 1,
        LayoutOrder = 3,
        Parent = card,
    })
    local bar = Create("Frame", {
        AnchorPoint = Vector2.new(0, 1),
        Position = UDim2.new(0, 0, 1, 0),
        Size = UDim2.new(1, 0, 0, 2),
        Theme = { BackgroundColor3 = "Accent" },
        Parent = barHolder,
    })
    Tween(card, { GroupTransparency = 0 }, 0.2)
    Tween(bar, { Size = UDim2.new(0, 0, 0, 2) }, time).Completed:Connect(function()
        local out = Tween(card, { GroupTransparency = 1 }, 0.25)
        out.Completed:Connect(function()
            card:Destroy()
        end)
    end)
end

function Library:SetWatermark(text)
    self:_Init()
    self.WatermarkFormat = text
    if not self.Watermark then
        local frame = Create("Frame", {
            Name = "Watermark",
            Position = UDim2.fromOffset(14, 14),
            Size = UDim2.fromOffset(0, 26),
            AutomaticSize = Enum.AutomaticSize.X,
            ZIndex = 5,
            Theme = { BackgroundColor3 = "Panel" },
            Parent = self.Root,
        })
        Corner(frame, 6)
        Stroke(frame)
        Pad(frame, 10, 0)
        self.WatermarkLabel = Create("TextLabel", {
            Size = UDim2.new(0, 0, 1, 0),
            AutomaticSize = Enum.AutomaticSize.X,
            Theme = { TextColor3 = "Text" },
            Parent = frame,
        })
        self.Watermark = frame
    end
    self.Watermark.Visible = text ~= nil
    self:_UpdateWatermark()
end

function Library:_UpdateWatermark()
    if self.Watermark and self.WatermarkFormat then
        local text = self.WatermarkFormat:gsub("{fps}", tostring(self.FPS)):gsub("{time}", os.date("%H:%M"))
        self.WatermarkLabel.Text = text
    end
end

function Library:_BuildHotkeys()
    if self.HotkeyHolder then
        return
    end
    local holder = Create("Frame", {
        Name = "Hotkeys",
        BackgroundTransparency = 1,
        Position = UDim2.new(0, 14, 0.38, 0),
        Size = UDim2.fromOffset(0, 0),
        AutomaticSize = Enum.AutomaticSize.XY,
        ZIndex = 5,
        Parent = self.Root,
    })
    List(holder, 5)
    local header = Create("Frame", {
        Size = UDim2.fromOffset(0, 26),
        AutomaticSize = Enum.AutomaticSize.X,
        LayoutOrder = 0,
        Theme = { BackgroundColor3 = "Panel" },
        Parent = holder,
    })
    Corner(header, 6)
    Stroke(header)
    Pad(header, 10, 0)
    local inner = Create("Frame", {
        BackgroundTransparency = 1,
        Size = UDim2.new(0, 0, 1, 0),
        AutomaticSize = Enum.AutomaticSize.X,
        Parent = header,
    })
    List(inner, 6, true)
    MakeIcon(inner, "key", 12, "SubText")
    Create("TextLabel", {
        Size = UDim2.new(0, 0, 1, 0),
        AutomaticSize = Enum.AutomaticSize.X,
        Text = "Hotkeys",
        FontFace = FontBold,
        Theme = { TextColor3 = "Text" },
        Parent = inner,
    })
    MakeDraggable(header, holder)
    self.HotkeyHolder = holder
end

function Library:SetHotkeysVisible(state)
    self:_BuildHotkeys()
    self.HotkeyHolder.Visible = state and true or false
end

function Library:UpdateHotkeys()
    if not self.HotkeyHolder then
        return
    end
    local seen = {}
    local order = 0
    for kb in pairs(self.Keybinds) do
        local active = kb.Key ~= nil and ((kb.Toggle and kb.Toggle.Value) or (not kb.Toggle and kb.State))
        if active then
            seen[kb] = true
            order = order + 1
            local entry = self.HotkeyEntries[kb]
            if not entry then
                local frame = Create("Frame", {
                    Size = UDim2.fromOffset(0, 24),
                    AutomaticSize = Enum.AutomaticSize.X,
                    Theme = { BackgroundColor3 = "Panel" },
                    Parent = self.HotkeyHolder,
                })
                Corner(frame, 6)
                Stroke(frame)
                Create("Frame", {
                    Size = UDim2.new(0, 2, 1, -10),
                    Position = UDim2.new(0, 0, 0, 5),
                    Theme = { BackgroundColor3 = "Accent" },
                    Parent = frame,
                })
                Pad(frame, 10, 0, 10, 0)
                local label = Create("TextLabel", {
                    Size = UDim2.new(0, 0, 1, 0),
                    AutomaticSize = Enum.AutomaticSize.X,
                    Theme = { TextColor3 = "Text" },
                    Parent = frame,
                })
                entry = { Frame = frame, Label = label }
                self.HotkeyEntries[kb] = entry
            end
            entry.Frame.LayoutOrder = order
            entry.Label.Text = (kb.Name or kb.Flag) .. " - " .. KeyName(kb.Key)
        end
    end
    for kb, entry in pairs(self.HotkeyEntries) do
        if not seen[kb] then
            entry.Frame:Destroy()
            self.HotkeyEntries[kb] = nil
        end
    end
end

function Library:Unload()
    if self.Unloaded then
        return
    end
    self.Unloaded = true
    for _, fn in ipairs(self.OnUnload) do
        pcall(fn)
    end
    for _, conn in ipairs(self.Connections) do
        pcall(function()
            conn:Disconnect()
        end)
    end
    if self.Root then
        self.Root:Destroy()
    end
    self.Animated = {}
    if Env.UILibrary == self then
        Env.UILibrary = nil
    end
end

function Library:_Ensure()
    if not (makefolder and isfolder) then
        return false
    end
    pcall(function()
        if not isfolder(self.Folder) then
            makefolder(self.Folder)
        end
        if not isfolder(self.Folder .. "/configs") then
            makefolder(self.Folder .. "/configs")
        end
    end)
    return true
end

local function EncodeKey(key)
    if not key then
        return false
    end
    return { Type = key.EnumType == Enum.KeyCode and "KeyCode" or "UserInputType", Name = key.Name }
end

local function DecodeKey(data)
    if type(data) ~= "table" then
        return nil
    end
    local ok, result = pcall(function()
        return Enum[data.Type][data.Name]
    end)
    return ok and result or nil
end

function Library:Serialize()
    local data = {}
    for flag, option in pairs(self.Options) do
        if not option.NoSave then
            local kind = option.Type
            if kind == "ColorPicker" then
                data[flag] = {
                    R = option.Value.R,
                    G = option.Value.G,
                    B = option.Value.B,
                    A = option.Alpha,
                    Anim = option.Animation,
                    Base = option.Base,
                }
            elseif kind == "Keybind" then
                data[flag] = { Key = EncodeKey(option.Key), Mode = option.Mode }
            elseif kind == "Toggle" or kind == "Slider" or kind == "RangeSlider" or kind == "Dropdown" or kind == "List" or kind == "Input" then
                data[flag] = option.Value
            end
        end
    end
    return data
end

function Library:Deserialize(data)
    for flag, value in pairs(data) do
        local option = self.Options[flag]
        if option and not option.NoSave then
            pcall(function()
                local kind = option.Type
                if kind == "ColorPicker" then
                    option.Base = value.Base or option.Base
                    option:Set(Color3.new(value.R, value.G, value.B), value.A)
                    option:SetAnimation(value.Anim or "None")
                elseif kind == "Keybind" then
                    option:Set(DecodeKey(value.Key), value.Mode)
                else
                    option:Set(value)
                end
            end)
        end
    end
end

function Library:SaveConfig(name)
    if not name or name == "" then
        return false, "Empty name"
    end
    if not (self:_Ensure() and writefile) then
        return false, "Filesystem unavailable"
    end
    local ok, err = pcall(function()
        writefile(self.Folder .. "/configs/" .. name .. ".json", HttpService:JSONEncode(self:Serialize()))
    end)
    return ok, err
end

function Library:LoadConfig(name)
    if not (self:_Ensure() and readfile and isfile) then
        return false, "Filesystem unavailable"
    end
    local path = self.Folder .. "/configs/" .. name .. ".json"
    if not isfile(path) then
        return false, "Config not found"
    end
    local ok, result = pcall(function()
        return HttpService:JSONDecode(readfile(path))
    end)
    if not ok then
        return false, result
    end
    self:Deserialize(result)
    return true
end

function Library:DeleteConfig(name)
    if not (delfile and isfile) then
        return false, "Filesystem unavailable"
    end
    local path = self.Folder .. "/configs/" .. name .. ".json"
    if not isfile(path) then
        return false, "Config not found"
    end
    return pcall(delfile, path)
end

function Library:GetConfigs()
    local result = {}
    if self:_Ensure() and listfiles then
        local ok, files = pcall(listfiles, self.Folder .. "/configs")
        if ok then
            for _, file in ipairs(files) do
                local name = file:match("([^/\\]+)%.json$")
                if name then
                    result[#result + 1] = name
                end
            end
        end
    end
    table.sort(result)
    return result
end

function Library:CreateWindow(opts)
    opts = opts or {}
    self:_Init()
    self:_BuildHotkeys()
    self.HotkeyHolder.Visible = opts.Hotkeys ~= false

    local size = opts.Size or Vector2.new(660, 450)
    local view = workspace.CurrentCamera.ViewportSize
    local win = setmetatable({
        Tabs = {},
        Library = self,
        Query = "",
        Visible = true,
        Order = 0,
        MenuKey = ParseKey(opts.Keybind or "RightShift"),
    }, Window)

    win.Holder = Create("Frame", {
        Name = "Window",
        BackgroundTransparency = 1,
        Position = UDim2.fromOffset(floor((view.X - size.X) / 2), floor((view.Y - size.Y) / 2) + 16),
        Size = UDim2.fromOffset(size.X, size.Y),
        Parent = self.Root,
    })

    local main = Create("Frame", {
        Size = UDim2.fromScale(1, 1),
        ClipsDescendants = true,
        Theme = { BackgroundColor3 = "Background" },
        Parent = win.Holder,
    })
    Corner(main, 8)
    Stroke(main)

    local info = Create("Frame", {
        Position = UDim2.new(0, 0, 0, -34),
        Size = UDim2.fromOffset(0, 26),
        AutomaticSize = Enum.AutomaticSize.X,
        Theme = { BackgroundColor3 = "Panel" },
        Parent = win.Holder,
    })
    Corner(info, 6)
    Stroke(info)
    Pad(info, 10, 0)
    local infoRow = Create("Frame", {
        BackgroundTransparency = 1,
        Size = UDim2.new(0, 0, 1, 0),
        AutomaticSize = Enum.AutomaticSize.X,
        Parent = info,
    })
    List(infoRow, 8, true)
    local infoItems = opts.Info or { tostring(opts.Title or "Window") }
    local user = opts.User
    if user == nil then
        user = LocalPlayer.DisplayName .. " (@" .. LocalPlayer.Name .. ")"
    end
    if user and user ~= false then
        infoItems = { table.unpack(infoItems) }
        infoItems[#infoItems + 1] = user
    end
    for i, text in ipairs(infoItems) do
        if i == 1 then
            MakeIcon(infoRow, opts.Icon or "flower", 12, "Text")
        else
            Create("Frame", {
                Size = UDim2.fromOffset(1, 12),
                Theme = { BackgroundColor3 = "Border" },
                LayoutOrder = i * 2 - 1,
                Parent = infoRow,
            })
        end
        Create("TextLabel", {
            Size = UDim2.new(0, 0, 1, 0),
            AutomaticSize = Enum.AutomaticSize.X,
            Text = text,
            LayoutOrder = i * 2,
            Theme = { TextColor3 = "Text" },
            Parent = infoRow,
        })
    end
    MakeDraggable(info, win.Holder)

    local sidebar = Create("Frame", {
        Size = UDim2.new(0, 156, 1, 0),
        Theme = { BackgroundColor3 = "Panel" },
        Parent = main,
    })
    Create("Frame", {
        AnchorPoint = Vector2.new(1, 0),
        Position = UDim2.fromScale(1, 0),
        Size = UDim2.new(0, 1, 1, 0),
        Theme = { BackgroundColor3 = "Border" },
        Parent = sidebar,
    })

    local logo = Create("Frame", {
        BackgroundTransparency = 1,
        Size = UDim2.new(1, 0, 0, 52),
        Parent = sidebar,
    })
    local logoRow = Create("Frame", {
        BackgroundTransparency = 1,
        Position = UDim2.fromOffset(16, 0),
        Size = UDim2.new(1, -16, 1, 0),
        Parent = logo,
    })
    List(logoRow, 8, true)
    MakeIcon(logoRow, opts.Icon or "flower", 18, "Text")
    win.TitleLabel = Create("TextLabel", {
        Size = UDim2.new(0, 0, 1, 0),
        AutomaticSize = Enum.AutomaticSize.X,
        Text = opts.Title or "Window",
        FontFace = FontBold,
        TextSize = 16,
        Theme = { TextColor3 = "Text" },
        Parent = logoRow,
    })
    MakeDraggable(logo, win.Holder)

    win.TabList = Create("ScrollingFrame", {
        Position = UDim2.fromOffset(0, 52),
        Size = UDim2.new(1, 0, 1, -96),
        ScrollBarThickness = 0,
        Parent = sidebar,
    })
    Pad(win.TabList, 8, 0, 8, 0)
    List(win.TabList, 2)

    local searchBox = Create("Frame", {
        AnchorPoint = Vector2.new(0, 1),
        Position = UDim2.new(0, 8, 1, -8),
        Size = UDim2.new(1, -16, 0, 28),
        Theme = { BackgroundColor3 = "Element" },
        Parent = sidebar,
    })
    Corner(searchBox, 6)
    Stroke(searchBox)
    MakeIcon(searchBox, "search", 14, "SubText", { Position = UDim2.fromOffset(6, 5) })
    local search = Create("TextBox", {
        Position = UDim2.fromOffset(28, 0),
        Size = UDim2.new(1, -34, 1, 0),
        PlaceholderText = "Search...",
        Text = "",
        ClearTextOnFocus = false,
        Theme = { TextColor3 = "Text", PlaceholderColor3 = "SubText" },
        Parent = searchBox,
    })
    search:GetPropertyChangedSignal("Text"):Connect(function()
        win.Query = string.lower(search.Text)
        win:_Filter()
    end)

    win.Content = Create("Frame", {
        BackgroundTransparency = 1,
        Position = UDim2.fromOffset(156, 0),
        Size = UDim2.new(1, -156, 1, 0),
        Parent = main,
    })
    Pad(win.Content, 10)

    if UserInputService.TouchEnabled and not UserInputService.KeyboardEnabled then
        local button = Create("TextButton", {
            Position = UDim2.new(0.5, -20, 0, 8),
            Size = UDim2.fromOffset(40, 40),
            Text = "☰",
            TextSize = 20,
            ZIndex = 20,
            BackgroundTransparency = 0,
            Theme = { BackgroundColor3 = "Panel", TextColor3 = "Text" },
            Parent = self.Root,
        })
        Corner(button, 20)
        Stroke(button)
        button.Activated:Connect(function()
            win:Toggle()
        end)
    end

    self.Windows[#self.Windows + 1] = win
    return win
end

function Window:Toggle(state)
    if state == nil then
        state = not self.Visible
    end
    self.Visible = state
    self.Holder.Visible = state
    self.Library:ClosePopup()
end

function Window:SetTitle(text)
    self.TitleLabel.Text = text
end

function Window:AddCategory(name)
    self.Order = self.Order + 1
    Create("TextLabel", {
        Size = UDim2.new(1, 0, 0, 22),
        Text = name,
        TextSize = 10,
        FontFace = FontBold,
        LayoutOrder = self.Order,
        Theme = { TextColor3 = "SubText" },
        Parent = self.TabList,
    })
end

function Window:SelectTab(tab)
    self.Library:ClosePopup()
    for _, other in ipairs(self.Tabs) do
        local active = other == tab
        other.Page.Visible = active
        Tween(other.Button, { BackgroundTransparency = active and 0 or 1 })
        other.Stroke.Enabled = active
        Tween(other.Name, { TextColor3 = active and self.Library.Theme.Text or self.Library.Theme.SubText })
        other.IconLabel[other.IconLabel:IsA("TextLabel") and "TextColor3" or "ImageColor3"] = active and self.Library.Theme.Text or self.Library.Theme.SubText
    end
    self.Active = tab
end

function Window:AddTab(opts)
    if type(opts) == "string" then
        opts = { Name = opts }
    end
    local lib = self.Library
    self.Order = self.Order + 1
    local tab = setmetatable({
        Window = self,
        Sections = {},
        Left = nil,
        Right = nil,
        Info = opts,
    }, Tab)
    tab.Button = Create("TextButton", {
        Size = UDim2.new(1, 0, 0, 28),
        LayoutOrder = self.Order,
        Theme = { BackgroundColor3 = "Element" },
        BackgroundTransparency = 1,
        Parent = self.TabList,
    })
    Corner(tab.Button, 6)
    tab.Stroke = Stroke(tab.Button)
    tab.Stroke.Enabled = false
    tab.IconLabel = MakeIcon(tab.Button, opts.Icon or "dot", 14, "SubText", { Position = UDim2.fromOffset(8, 6) })
    tab.Name = Create("TextLabel", {
        Position = UDim2.fromOffset(32, 0),
        Size = UDim2.new(1, -36, 1, 0),
        Text = opts.Name or "Tab",
        Theme = { TextColor3 = "SubText" },
        Parent = tab.Button,
    })
    tab.Page = Create("Frame", {
        BackgroundTransparency = 1,
        Size = UDim2.fromScale(1, 1),
        Visible = false,
        Parent = self.Content,
    })
    local function column(x)
        local col = Create("ScrollingFrame", {
            Position = UDim2.new(x, x == 0 and 0 or 5, 0, 0),
            Size = UDim2.new(0.5, -5, 1, 0),
            Parent = tab.Page,
        })
        Pad(col, 0, 0, 6, 0)
        List(col, 10)
        return col
    end
    tab.Left = column(0)
    tab.Right = column(0.5)
    tab.Button.MouseEnter:Connect(function()
        if self.Active ~= tab then
            Tween(tab.Name, { TextColor3 = lib.Theme.Text })
        end
    end)
    tab.Button.MouseLeave:Connect(function()
        if self.Active ~= tab then
            Tween(tab.Name, { TextColor3 = lib.Theme.SubText })
        end
    end)
    tab.Button.Activated:Connect(function()
        self:SelectTab(tab)
    end)
    self.Tabs[#self.Tabs + 1] = tab
    if #self.Tabs == 1 then
        self:SelectTab(tab)
    end
    return tab
end

function Tab:AddSection(opts)
    if type(opts) == "string" then
        opts = { Name = opts }
    end
    local side = opts.Side
    local parent
    if side == "Right" then
        parent = self.Right
    elseif side == "Left" then
        parent = self.Left
    else
        local l, r = 0, 0
        for _, s in ipairs(self.Sections) do
            if s.Parent == self.Left then
                l = l + 1
            else
                r = r + 1
            end
        end
        parent = l <= r and self.Left or self.Right
    end
    local section = setmetatable({ Tab = self, Items = {}, Parent = parent }, Section)
    section.Frame = Create("Frame", {
        BackgroundTransparency = 1,
        Size = UDim2.new(1, 0, 0, 0),
        AutomaticSize = Enum.AutomaticSize.Y,
        LayoutOrder = #self.Sections + 1,
        Parent = parent,
    })
    List(section.Frame, 6)
    local header = Create("Frame", {
        BackgroundTransparency = 1,
        Size = UDim2.new(1, 0, 0, 16),
        LayoutOrder = 0,
        Parent = section.Frame,
    })
    List(header, 6, true)
    if opts.Icon then
        MakeIcon(header, opts.Icon, 13, "SubText")
    end
    Create("TextLabel", {
        Size = UDim2.new(0, 0, 1, 0),
        AutomaticSize = Enum.AutomaticSize.X,
        Text = opts.Name or "Section",
        FontFace = FontBold,
        Theme = { TextColor3 = "SubText" },
        Parent = header,
    })
    if opts.Description then
        Create("TextLabel", {
            Size = UDim2.new(1, 0, 0, 14),
            Text = opts.Description,
            TextSize = 11,
            LayoutOrder = 1,
            Theme = { TextColor3 = "SubText" },
            Parent = section.Frame,
        })
    end
    section.Box = Create("Frame", {
        Size = UDim2.new(1, 0, 0, 0),
        AutomaticSize = Enum.AutomaticSize.Y,
        LayoutOrder = 2,
        Theme = { BackgroundColor3 = "Panel" },
        Parent = section.Frame,
    })
    Corner(section.Box, 6)
    Stroke(section.Box)
    Pad(section.Box, 12)
    List(section.Box, 9)
    self.Sections[#self.Sections + 1] = section
    return section
end

function Window:_Filter()
    local q = self.Query
    local firstMatch
    for _, tab in ipairs(self.Tabs) do
        local any = false
        for _, section in ipairs(tab.Sections) do
            local secAny = false
            for _, entry in ipairs(section.Items) do
                local match = q == "" or entry.Text:find(q, 1, true) ~= nil
                local shown = entry.Visible and match
                entry.Row.Visible = shown
                if shown then
                    secAny = true
                end
            end
            section.Frame.Visible = secAny or #section.Items == 0
            if secAny and q ~= "" then
                any = true
            end
        end
        tab.Button.Visible = q == "" or any
        if any and not firstMatch then
            firstMatch = tab
        end
    end
    if q ~= "" and firstMatch and (not self.Active or not self.Active.Button.Visible) then
        self:SelectTab(firstMatch)
    end
end

function Section:_Row(height, text)
    local row = Create("Frame", {
        BackgroundTransparency = 1,
        Size = UDim2.new(1, 0, 0, height),
        LayoutOrder = #self.Items + 1,
        Parent = self.Box,
    })
    local entry = { Row = row, Text = string.lower(text or ""), Visible = true }
    self.Items[#self.Items + 1] = entry
    return row, entry
end

function Section:_Register(ctrl, flag, entry, opts)
    ctrl.Flag = flag
    ctrl.Entry = entry
    ctrl.Section = self
    ctrl.Callbacks = {}
    ctrl.NoSave = opts.NoSave
    if opts.Callback then
        ctrl.Callbacks[1] = opts.Callback
    end
    if flag then
        Library.Options[flag] = ctrl
    end
    if opts.Tooltip then
        ctrl.Tooltip = opts.Tooltip
    end
    return ctrl
end

function Section:AddLabel(text, opts)
    opts = opts or {}
    local label = Create("TextLabel", {
        Size = UDim2.new(1, 0, 0, 14),
        AutomaticSize = Enum.AutomaticSize.Y,
        Text = text,
        TextWrapped = opts.Wrap ~= false,
        TextSize = opts.Size or 12,
        FontFace = opts.Bold and FontBold or FontRegular,
        LayoutOrder = #self.Items + 1,
        Theme = { TextColor3 = opts.Dim and "SubText" or "Text" },
        Parent = self.Box,
    })
    local entry = { Row = label, Text = string.lower(text), Visible = true }
    self.Items[#self.Items + 1] = entry
    local ctrl = setmetatable({ Type = "Label", Callbacks = {} }, LabelControl)
    ctrl.Entry, ctrl.Section = entry, self
    function ctrl:SetText(value)
        label.Text = value
        entry.Text = string.lower(value)
    end
    return ctrl
end

function Section:AddDivider(text)
    local row, entry = self:_Row(12, text)
    local line = Create("Frame", {
        AnchorPoint = Vector2.new(0, 0.5),
        Position = UDim2.fromScale(0, 0.5),
        Size = UDim2.new(1, 0, 0, 1),
        Theme = { BackgroundColor3 = "Border" },
        Parent = row,
    })
    if text then
        local label = Create("TextLabel", {
            AnchorPoint = Vector2.new(0.5, 0.5),
            Position = UDim2.fromScale(0.5, 0.5),
            Size = UDim2.fromOffset(0, 12),
            AutomaticSize = Enum.AutomaticSize.X,
            Text = " " .. text .. " ",
            TextSize = 10,
            Theme = { TextColor3 = "SubText", BackgroundColor3 = "Panel" },
            BackgroundTransparency = 0,
            ZIndex = 2,
            Parent = row,
        })
        label.TextXAlignment = Enum.TextXAlignment.Center
    end
    return entry
end

function Toggle:_Visual(instant)
    local theme = Library.Theme
    local on = self.Value
    local props = {
        Switch = { BackgroundColor3 = on and theme.Accent or theme.Element },
        Knob = {
            Position = on and UDim2.new(1, -11, 0.5, -4) or UDim2.new(0, 3, 0.5, -4),
            BackgroundColor3 = on and theme.OnAccent or theme.SubText,
        },
        Label = { TextColor3 = on and theme.Text or theme.SubText },
    }
    for name, p in pairs(props) do
        if instant then
            for k, v in pairs(p) do
                self[name][k] = v
            end
        else
            Tween(self[name], p)
        end
    end
end

function Toggle:Set(value, silent)
    value = value and true or false
    self.Value = value
    Library.Flags[self.Flag] = value
    self:_Visual()
    if not silent then
        self:Fire(value)
    end
    Library:UpdateHotkeys()
end

function Toggle:AddKeybind(flag, opts)
    self.AddonCount = self.AddonCount + 1
    Library:_Keybind(flag, opts or {}, self.Right, self, self.Section, self.AddonCount, self.Text)
    return self
end

function Toggle:AddColorPicker(flag, opts)
    self.AddonCount = self.AddonCount + 1
    Library:_ColorPicker(flag, opts or {}, self.Right, self.Section, self.AddonCount)
    return self
end

function Section:AddToggle(flag, opts)
    opts = opts or {}
    local text = opts.Text or flag
    local row, entry = self:_Row(20, text)
    local toggle = setmetatable({ Type = "Toggle", Value = false, AddonCount = 0, Text = text }, Toggle)
    self:_Register(toggle, flag, entry, opts)
    toggle.Label = Create("TextLabel", {
        Size = UDim2.new(1, -44, 1, 0),
        Text = text,
        TextTruncate = Enum.TextTruncate.AtEnd,
        TextColor3 = Library.Theme.SubText,
        Parent = row,
    })
    toggle.Right = Create("Frame", {
        BackgroundTransparency = 1,
        AnchorPoint = Vector2.new(1, 0.5),
        Position = UDim2.new(1, 0, 0.5, 0),
        Size = UDim2.new(0, 0, 1, 0),
        AutomaticSize = Enum.AutomaticSize.X,
        ZIndex = 2,
        Parent = row,
    })
    List(toggle.Right, 6, true, Enum.HorizontalAlignment.Right)
    toggle.Switch = Create("Frame", {
        Size = UDim2.fromOffset(28, 14),
        LayoutOrder = 1000,
        Parent = toggle.Right,
    })
    Corner(toggle.Switch, 7)
    Stroke(toggle.Switch)
    toggle.Knob = Create("Frame", {
        Size = UDim2.fromOffset(8, 8),
        Position = UDim2.new(0, 3, 0.5, -4),
        Parent = toggle.Switch,
    })
    Corner(toggle.Knob, 4)
    local button = Create("TextButton", { Size = UDim2.fromScale(1, 1), Parent = row })
    button.Activated:Connect(function()
        toggle:Set(not toggle.Value)
    end)
    toggle.Value = opts.Default and true or false
    Library.Flags[flag] = toggle.Value
    toggle:_Visual(true)
    return toggle
end

function Slider:_Visual()
    local ratio = (self.Value - self.Min) / max(self.Max - self.Min, 1e-9)
    Tween(self.Fill, { Size = UDim2.new(ratio, 0, 1, 0) }, 0.06)
    self.Box.Text = string.format("%." .. self.Dec .. "f", self.Value) .. self.Suffix
end

function Slider:Set(value, silent)
    value = tonumber(value) or self.Min
    value = clamp(floor(value / self.Inc + 0.5) * self.Inc, self.Min, self.Max)
    value = tonumber(string.format("%." .. self.Dec .. "f", value))
    if value == self.Value and self.Initialized then
        self:_Visual()
        return
    end
    self.Initialized = true
    self.Value = value
    Library.Flags[self.Flag] = value
    self:_Visual()
    if not silent then
        self:Fire(value)
    end
end

function Section:AddSlider(flag, opts)
    opts = opts or {}
    local text = opts.Text or flag
    local row, entry = self:_Row(36, text)
    local slider = setmetatable({
        Type = "Slider",
        Min = opts.Min or 0,
        Max = opts.Max or 100,
        Inc = opts.Increment or opts.Rounding or 1,
        Suffix = opts.Suffix or "",
    }, Slider)
    local dec = tostring(slider.Inc):match("%.(%d+)")
    slider.Dec = dec and #dec or 0
    self:_Register(slider, flag, entry, opts)
    Create("TextLabel", {
        Size = UDim2.new(1, -60, 0, 16),
        Text = text,
        Theme = { TextColor3 = "Text" },
        Parent = row,
    })
    slider.Box = Create("TextBox", {
        AnchorPoint = Vector2.new(1, 0),
        Position = UDim2.fromScale(1, 0),
        Size = UDim2.fromOffset(0, 16),
        AutomaticSize = Enum.AutomaticSize.X,
        TextSize = 10,
        TextXAlignment = Enum.TextXAlignment.Center,
        BackgroundTransparency = 0,
        ClearTextOnFocus = true,
        Theme = { BackgroundColor3 = "Element", TextColor3 = "SubText" },
        Parent = row,
    })
    Corner(slider.Box, 4)
    Stroke(slider.Box)
    Pad(slider.Box, 5, 0)
    Create("UISizeConstraint", { MinSize = Vector2.new(36, 16), Parent = slider.Box })
    local hit = Create("Frame", {
        BackgroundTransparency = 1,
        Position = UDim2.fromOffset(0, 20),
        Size = UDim2.new(1, 0, 0, 14),
        Parent = row,
    })
    local bar = Create("Frame", {
        AnchorPoint = Vector2.new(0, 0.5),
        Position = UDim2.fromScale(0, 0.5),
        Size = UDim2.new(1, 0, 0, 4),
        Theme = { BackgroundColor3 = "Element" },
        Parent = hit,
    })
    Corner(bar, 2)
    Stroke(bar)
    slider.Fill = Create("Frame", {
        Size = UDim2.fromScale(0, 1),
        Theme = { BackgroundColor3 = "Accent" },
        Parent = bar,
    })
    Corner(slider.Fill, 2)
    local knob = Create("Frame", {
        AnchorPoint = Vector2.new(0.5, 0.5),
        Position = UDim2.fromScale(1, 0.5),
        Size = UDim2.fromOffset(10, 10),
        Theme = { BackgroundColor3 = "Accent" },
        Parent = slider.Fill,
    })
    Corner(knob, 5)
    Stroke(knob, "Background", 2)
    Track(hit, function(pt)
        local ratio = clamp((pt.X - bar.AbsolutePosition.X) / max(bar.AbsoluteSize.X, 1), 0, 1)
        slider:Set(slider.Min + (slider.Max - slider.Min) * ratio)
    end)
    slider.Box.FocusLost:Connect(function()
        local n = tonumber(slider.Box.Text:match("-?%d+%.?%d*"))
        slider:Set(n or slider.Value)
    end)
    slider:Set(opts.Default or slider.Min, true)
    return slider
end

function RangeSlider:_Visual()
    local span = max(self.Max - self.Min, 1e-9)
    local lo, hi = (self.Value[1] - self.Min) / span, (self.Value[2] - self.Min) / span
    Tween(self.Fill, { Position = UDim2.fromScale(lo, 0), Size = UDim2.new(hi - lo, 0, 1, 0) }, 0.06)
    Tween(self.KnobLo, { Position = UDim2.fromScale(lo, 0.5) }, 0.06)
    Tween(self.KnobHi, { Position = UDim2.fromScale(hi, 0.5) }, 0.06)
    local fmt = "%." .. self.Dec .. "f"
    self.BoxLo.Text = string.format(fmt, self.Value[1]) .. self.Suffix
    self.BoxHi.Text = string.format(fmt, self.Value[2]) .. self.Suffix
end

function RangeSlider:Set(value, silent)
    local lo, hi = self.Min, self.Max
    if type(value) == "table" then
        lo, hi = tonumber(value[1]) or lo, tonumber(value[2]) or hi
    end
    local function snap(v)
        v = clamp(floor(v / self.Inc + 0.5) * self.Inc, self.Min, self.Max)
        return tonumber(string.format("%." .. self.Dec .. "f", v))
    end
    lo, hi = snap(lo), snap(hi)
    if lo > hi then
        lo, hi = hi, lo
    end
    self.Value = { lo, hi }
    Library.Flags[self.Flag] = self.Value
    self:_Visual()
    if not silent then
        self:Fire(lo, hi)
    end
end

function Section:AddRangeSlider(flag, opts)
    opts = opts or {}
    local text = opts.Text or flag
    local row, entry = self:_Row(36, text)
    local slider = setmetatable({
        Type = "RangeSlider",
        Min = opts.Min or 0,
        Max = opts.Max or 100,
        Inc = opts.Increment or opts.Rounding or 1,
        Suffix = opts.Suffix or "",
        Value = { 0, 0 },
    }, RangeSlider)
    local dec = tostring(slider.Inc):match("%.(%d+)")
    slider.Dec = dec and #dec or 0
    self:_Register(slider, flag, entry, opts)
    Create("TextLabel", {
        Size = UDim2.new(1, -100, 0, 16),
        Text = text,
        Theme = { TextColor3 = "Text" },
        Parent = row,
    })
    local boxes = Create("Frame", {
        BackgroundTransparency = 1,
        AnchorPoint = Vector2.new(1, 0),
        Position = UDim2.fromScale(1, 0),
        Size = UDim2.fromOffset(0, 16),
        AutomaticSize = Enum.AutomaticSize.X,
        Parent = row,
    })
    List(boxes, 4, true)
    local function chip(order)
        local label = Create("TextLabel", {
            Size = UDim2.fromOffset(0, 16),
            AutomaticSize = Enum.AutomaticSize.X,
            TextSize = 10,
            TextXAlignment = Enum.TextXAlignment.Center,
            BackgroundTransparency = 0,
            LayoutOrder = order,
            Theme = { BackgroundColor3 = "Element", TextColor3 = "SubText" },
            Parent = boxes,
        })
        Corner(label, 4)
        Stroke(label)
        Pad(label, 5, 0)
        Create("UISizeConstraint", { MinSize = Vector2.new(34, 16), Parent = label })
        return label
    end
    slider.BoxLo, slider.BoxHi = chip(1), chip(2)
    local hit = Create("Frame", {
        BackgroundTransparency = 1,
        Position = UDim2.fromOffset(0, 20),
        Size = UDim2.new(1, 0, 0, 14),
        Parent = row,
    })
    local bar = Create("Frame", {
        AnchorPoint = Vector2.new(0, 0.5),
        Position = UDim2.fromScale(0, 0.5),
        Size = UDim2.new(1, 0, 0, 4),
        Theme = { BackgroundColor3 = "Element" },
        Parent = hit,
    })
    Corner(bar, 2)
    Stroke(bar)
    slider.Fill = Create("Frame", {
        Size = UDim2.fromScale(0, 1),
        Theme = { BackgroundColor3 = "Accent" },
        Parent = bar,
    })
    Corner(slider.Fill, 2)
    local function knob()
        local k = Create("Frame", {
            AnchorPoint = Vector2.new(0.5, 0.5),
            Size = UDim2.fromOffset(10, 10),
            Theme = { BackgroundColor3 = "Accent" },
            Parent = bar,
        })
        Corner(k, 5)
        Stroke(k, "Background", 2)
        return k
    end
    slider.KnobLo, slider.KnobHi = knob(), knob()
    local which
    Track(hit, function(pt)
        local ratio = clamp((pt.X - bar.AbsolutePosition.X) / max(bar.AbsoluteSize.X, 1), 0, 1)
        local value = slider.Min + (slider.Max - slider.Min) * ratio
        if not which then
            local dLo, dHi = math.abs(value - slider.Value[1]), math.abs(value - slider.Value[2])
            if dLo == dHi then
                which = value > slider.Value[2] and 2 or 1
            else
                which = dLo < dHi and 1 or 2
            end
        end
        local lo, hi = slider.Value[1], slider.Value[2]
        if which == 1 then
            lo = min(value, hi)
        else
            hi = max(value, lo)
        end
        slider:Set({ lo, hi })
    end, function()
        which = nil
    end)
    slider:Set(opts.Default or { slider.Min, slider.Max }, true)
    return slider
end

function Dropdown:_Text()
    if self.Multi then
        local names = {}
        for _, v in ipairs(self.Values) do
            if self.Value[v] then
                names[#names + 1] = tostring(v)
            end
        end
        return #names > 0 and table.concat(names, ", ") or (self.Placeholder or "None")
    end
    return self.Value ~= nil and tostring(self.Value) or (self.Placeholder or "None")
end

function Dropdown:_Refresh()
    if not self.List then
        return
    end
    for _, child in ipairs(self.List:GetChildren()) do
        if child:IsA("TextButton") then
            child:Destroy()
        end
    end
    local lib = Library
    for i, value in ipairs(self.Values) do
        local selected
        if self.Multi then
            selected = self.Value[value] == true
        else
            selected = self.Value == value
        end
        local item = Create("TextButton", {
            Size = UDim2.new(1, 0, 0, 22),
            LayoutOrder = i,
            Text = tostring(value),
            TextXAlignment = Enum.TextXAlignment.Left,
            TextColor3 = selected and lib.Theme.Text or lib.Theme.SubText,
            FontFace = selected and FontBold or FontRegular,
            BackgroundTransparency = 1,
            BackgroundColor3 = lib.Theme.Hover,
            Parent = self.List,
        })
        Corner(item, 4)
        Pad(item, 10, 0)
        if selected then
            Create("Frame", {
                AnchorPoint = Vector2.new(0, 0.5),
                Position = UDim2.new(0, -6, 0.5, 0),
                Size = UDim2.fromOffset(2, 10),
                BackgroundColor3 = lib.Theme.Accent,
                Parent = item,
            })
        end
        item.MouseEnter:Connect(function()
            Tween(item, { BackgroundTransparency = 0 }, 0.1)
        end)
        item.MouseLeave:Connect(function()
            Tween(item, { BackgroundTransparency = 1 }, 0.1)
        end)
        item.Activated:Connect(function()
            if self.Multi then
                local set = {}
                for k, v in pairs(self.Value) do
                    set[k] = v
                end
                set[value] = not set[value] or nil
                self:Set(set)
            else
                self:Set(value)
                lib:ClosePopup()
            end
        end)
    end
end

function Dropdown:Set(value, silent)
    if self.Multi then
        local set = {}
        if type(value) == "table" then
            for k, v in pairs(value) do
                if type(k) == "number" then
                    set[v] = true
                elseif v then
                    set[k] = true
                end
            end
        end
        self.Value = set
    else
        self.Value = value
    end
    Library.Flags[self.Flag] = self.Value
    self.Display.Text = self:_Text()
    self:_Refresh()
    if not silent then
        self:Fire(self.Value)
    end
end

function Dropdown:SetValues(values, keep)
    self.Values = values
    if not keep then
        self:Set(self.Multi and {} or values[1], true)
    else
        self:_Refresh()
    end
end

function Dropdown:_Open()
    local lib = Library
    if lib:IsPopupOpen(self.Popup) then
        lib:ClosePopup()
        return
    end
    if not self.Popup then
        self.Popup = Create("Frame", {
            Visible = false,
            Theme = { BackgroundColor3 = "Panel" },
            Parent = lib.Popups,
        })
        Corner(self.Popup, 6)
        Stroke(self.Popup)
        self.List = Create("ScrollingFrame", {
            Position = UDim2.fromOffset(6, 4),
            Size = UDim2.new(1, -12, 1, -8),
            Parent = self.Popup,
        })
        Pad(self.List, 6, 0, 0, 0)
        List(self.List, 2)
    end
    self:_Refresh()
    self.Popup.Size = UDim2.fromOffset(self.Box.AbsoluteSize.X, min(#self.Values, 7) * 24 + 6)
    lib:Place(self.Popup, self.Box)
    Tween(self.Arrow, { Rotation = 180 })
    lib:OpenPopup(self.Popup, self.Box, function()
        Tween(self.Arrow, { Rotation = 0 })
    end)
end

function Section:AddDropdown(flag, opts)
    opts = opts or {}
    local row, entry = self:_Row(opts.Text and 44 or 24, opts.Text)
    local dd = setmetatable({
        Type = "Dropdown",
        Values = opts.Values or {},
        Multi = opts.Multi,
        Placeholder = opts.Placeholder,
    }, Dropdown)
    self:_Register(dd, flag, entry, opts)
    if opts.Text then
        Create("TextLabel", {
            Size = UDim2.new(1, 0, 0, 14),
            Text = opts.Text,
            Theme = { TextColor3 = "Text" },
            Parent = row,
        })
    end
    dd.Box = Create("TextButton", {
        Position = UDim2.fromOffset(0, opts.Text and 18 or 0),
        Size = UDim2.new(1, 0, 0, 24),
        BackgroundTransparency = 0,
        Theme = { BackgroundColor3 = "Element" },
        Parent = row,
    })
    Corner(dd.Box, 5)
    Stroke(dd.Box)
    dd.Display = Create("TextLabel", {
        Position = UDim2.fromOffset(9, 0),
        Size = UDim2.new(1, -30, 1, 0),
        TextTruncate = Enum.TextTruncate.AtEnd,
        Theme = { TextColor3 = "SubText" },
        Parent = dd.Box,
    })
    dd.Arrow = Create("TextLabel", {
        AnchorPoint = Vector2.new(1, 0.5),
        Position = UDim2.new(1, -6, 0.5, 0),
        Size = UDim2.fromOffset(14, 14),
        Text = "▾",
        TextSize = 12,
        TextXAlignment = Enum.TextXAlignment.Center,
        Theme = { TextColor3 = "SubText" },
        Parent = dd.Box,
    })
    dd.Box.MouseEnter:Connect(function()
        Tween(dd.Box, { BackgroundColor3 = Library.Theme.Hover })
    end)
    dd.Box.MouseLeave:Connect(function()
        Tween(dd.Box, { BackgroundColor3 = Library.Theme.Element })
    end)
    dd.Box.Activated:Connect(function()
        dd:_Open()
    end)
    local default = opts.Default
    if default == nil and not opts.Multi and not opts.AllowNull then
        default = dd.Values[1]
    end
    dd:Set(default or (opts.Multi and {} or nil), true)
    return dd
end

function ListBox:_Refresh()
    local lib = Library
    for _, item in ipairs(self.Items) do
        local selected = item.Value == self.Value
        Tween(item.Button, { BackgroundTransparency = selected and 0 or 1 }, 0.12)
        Tween(item.Label, { TextColor3 = selected and lib.Theme.Text or lib.Theme.SubText }, 0.12)
        item.Bar.Visible = selected
    end
end

function ListBox:Set(value, silent)
    self.Value = value
    Library.Flags[self.Flag] = value
    self:_Refresh()
    if not silent then
        self:Fire(value)
    end
end

function Section:AddList(flag, opts)
    opts = opts or {}
    local values = opts.Values or {}
    local row, entry = self:_Row(#values * 24 + 2, opts.Text or flag)
    local list = setmetatable({ Type = "List", Items = {} }, ListBox)
    self:_Register(list, flag, entry, opts)
    local frame = Create("Frame", {
        Size = UDim2.fromScale(1, 1),
        ClipsDescendants = true,
        Theme = { BackgroundColor3 = "Element" },
        Parent = row,
    })
    Corner(frame, 5)
    Stroke(frame)
    List(frame, 0)
    for i, value in ipairs(values) do
        local button = Create("TextButton", {
            Size = UDim2.new(1, 0, 0, 24),
            LayoutOrder = i,
            BackgroundTransparency = 1,
            Theme = { BackgroundColor3 = "Hover" },
            Parent = frame,
        })
        local bar = Create("Frame", {
            Size = UDim2.new(0, 2, 1, -10),
            Position = UDim2.fromOffset(2, 5),
            Visible = false,
            Theme = { BackgroundColor3 = "Accent" },
            Parent = button,
        })
        local label = Create("TextLabel", {
            Position = UDim2.fromOffset(10, 0),
            Size = UDim2.new(1, -14, 1, 0),
            Text = tostring(value),
            TextColor3 = Library.Theme.SubText,
            Parent = button,
        })
        button.Activated:Connect(function()
            list:Set(value)
        end)
        button.MouseEnter:Connect(function()
            if list.Value ~= value then
                Tween(label, { TextColor3 = Library.Theme.Text }, 0.1)
            end
        end)
        button.MouseLeave:Connect(function()
            if list.Value ~= value then
                Tween(label, { TextColor3 = Library.Theme.SubText }, 0.1)
            end
        end)
        list.Items[i] = { Value = value, Button = button, Label = label, Bar = bar }
    end
    list:Set(opts.Default or values[1], true)
    return list
end

function ButtonControl:SetText(text)
    self.Button.Text = text
end

function Section:AddButton(opts, callback)
    if type(opts) == "string" then
        opts = { Text = opts, Callback = callback }
    end
    opts = opts or {}
    local row, entry = self:_Row(26, opts.Text)
    local ctrl = setmetatable({ Type = "Button" }, ButtonControl)
    self:_Register(ctrl, nil, entry, opts)
    ctrl.Button = Create("TextButton", {
        Size = UDim2.fromScale(1, 1),
        Text = opts.Text or "Button",
        BackgroundTransparency = 0,
        Theme = { BackgroundColor3 = "Element", TextColor3 = "Text" },
        Parent = row,
    })
    Corner(ctrl.Button, 5)
    Stroke(ctrl.Button)
    ctrl.Button.MouseEnter:Connect(function()
        Tween(ctrl.Button, { BackgroundColor3 = Library.Theme.Hover })
    end)
    ctrl.Button.MouseLeave:Connect(function()
        Tween(ctrl.Button, { BackgroundColor3 = Library.Theme.Element })
    end)
    local confirming
    ctrl.Button.Activated:Connect(function()
        if opts.Confirm and not confirming then
            confirming = true
            ctrl.Button.Text = "Click again"
            task.delay(2, function()
                if confirming then
                    confirming = false
                    ctrl.Button.Text = opts.Text or "Button"
                end
            end)
            return
        end
        confirming = false
        ctrl.Button.Text = opts.Text or "Button"
        ctrl.Button.BackgroundColor3 = Library.Theme.Accent
        ctrl.Button.TextColor3 = Library.Theme.OnAccent
        Tween(ctrl.Button, { BackgroundColor3 = Library.Theme.Hover }, 0.25)
        Tween(ctrl.Button, { TextColor3 = Library.Theme.Text }, 0.25)
        ctrl:Fire()
    end)
    return ctrl
end

function Input:Set(value, silent)
    value = tostring(value or "")
    self.Value = value
    self.Box.Text = value
    Library.Flags[self.Flag] = value
    if not silent then
        self:Fire(value)
    end
end

function Section:AddInput(flag, opts)
    opts = opts or {}
    local row, entry = self:_Row(opts.Text and 44 or 26, opts.Text)
    local input = setmetatable({ Type = "Input", Numeric = opts.Numeric }, Input)
    self:_Register(input, flag, entry, opts)
    if opts.Text then
        Create("TextLabel", {
            Size = UDim2.new(1, 0, 0, 14),
            Text = opts.Text,
            Theme = { TextColor3 = "Text" },
            Parent = row,
        })
    end
    local holder = Create("Frame", {
        Position = UDim2.fromOffset(0, opts.Text and 18 or 0),
        Size = UDim2.new(1, 0, 0, 24),
        Theme = { BackgroundColor3 = "Element" },
        Parent = row,
    })
    Corner(holder, 5)
    local stroke = Stroke(holder)
    input.Box = Create("TextBox", {
        Position = UDim2.fromOffset(9, 0),
        Size = UDim2.new(1, -18, 1, 0),
        PlaceholderText = opts.Placeholder or "",
        ClearTextOnFocus = opts.ClearOnFocus or false,
        Text = "",
        Theme = { TextColor3 = "Text", PlaceholderColor3 = "SubText" },
        Parent = holder,
    })
    input.Box.Focused:Connect(function()
        Tween(stroke, { Color = Library.Theme.Accent })
    end)
    input.Box.FocusLost:Connect(function()
        Tween(stroke, { Color = Library.Theme.Border })
        local text = input.Box.Text
        if input.Numeric then
            text = tostring(tonumber(text) or input.Value or "")
        end
        input:Set(text)
    end)
    input:Set(opts.Default, true)
    return input
end

function Keybind:_Text()
    self.Button.Text = self.Listening and "..." or KeyName(self.Key)
end

function Keybind:Set(key, mode, silent)
    self.Key = ParseKey(key)
    if mode == "Toggle" or mode == "Hold" then
        self.Mode = mode
    end
    self.Listening = false
    if Library.Listening == self then
        Library.Listening = nil
    end
    self:_Text()
    if self.OnKey then
        self.OnKey(self.Key)
    end
    if not silent then
        self:Fire(self.State, self.Key)
    end
    Library:UpdateHotkeys()
end

function Keybind:SetState(state)
    state = state and true or false
    self.State = state
    Library.Flags[self.Flag] = state
    if self.Toggle then
        self.Toggle:Set(state)
    else
        self:Fire(state)
    end
    Library:UpdateHotkeys()
end

function Keybind:Get()
    return self.Key, self.Mode, self.State
end

function Keybind:_Capture(input)
    local utype = input.UserInputType
    local key
    if input.KeyCode ~= Enum.KeyCode.Unknown then
        key = input.KeyCode
    elseif utype == Enum.UserInputType.MouseButton1 or utype == Enum.UserInputType.MouseButton2 or utype == Enum.UserInputType.MouseButton3 then
        key = utype
    else
        return
    end
    if key == Enum.KeyCode.Escape then
        key = nil
    end
    self:Set(key, nil, true)
    self.State = false
    Library.Flags[self.Flag] = false
end

function Keybind:_Listen()
    local current = Library.Listening
    if current then
        current.Listening = false
        current:_Text()
    end
    self.Listening = true
    Library.Listening = self
    self:_Text()
end

function Keybind:_OpenMenu()
    local lib = Library
    if lib:IsPopupOpen(self.Menu) then
        lib:ClosePopup()
        return
    end
    if not self.Menu then
        self.Menu = Create("Frame", {
            Visible = false,
            Size = UDim2.fromOffset(160, 70),
            Theme = { BackgroundColor3 = "Panel" },
            Parent = lib.Popups,
        })
        Corner(self.Menu, 6)
        Stroke(self.Menu)
        Pad(self.Menu, 6)
        List(self.Menu, 6)
        local modes = Create("Frame", {
            BackgroundTransparency = 1,
            Size = UDim2.new(1, 0, 0, 24),
            LayoutOrder = 1,
            Parent = self.Menu,
        })
        List(modes, 4, true)
        self.ModeButtons = {}
        for _, mode in ipairs({ "Toggle", "Hold" }) do
            local button = Create("TextButton", {
                Size = UDim2.new(0.5, -2, 1, 0),
                Text = "On " .. mode,
                BackgroundTransparency = 0,
                Parent = modes,
            })
            Corner(button, 5)
            Stroke(button)
            button.Activated:Connect(function()
                self.Mode = mode
                self.State = false
                if self.Toggle and mode == "Hold" then
                    self.Toggle:Set(false)
                end
                self:_ModeVisual()
                self:Fire(self.State, self.Key)
                Library:UpdateHotkeys()
            end)
            self.ModeButtons[mode] = button
        end
        local reset = Create("TextButton", {
            Size = UDim2.new(1, 0, 0, 24),
            Text = "Reset",
            BackgroundTransparency = 0,
            LayoutOrder = 2,
            Theme = { BackgroundColor3 = "Element", TextColor3 = "Text" },
            Parent = self.Menu,
        })
        Corner(reset, 5)
        Stroke(reset)
        reset.Activated:Connect(function()
            self:Set(nil, "Toggle")
            self:_ModeVisual()
            lib:ClosePopup()
        end)
    end
    self:_ModeVisual()
    lib:Place(self.Menu, self.Button, true)
    lib:OpenPopup(self.Menu, self.Button)
end

function Keybind:_ModeVisual()
    local theme = Library.Theme
    for mode, button in pairs(self.ModeButtons) do
        local active = self.Mode == mode
        button.BackgroundColor3 = active and theme.Accent or theme.Element
        button.TextColor3 = active and theme.OnAccent or theme.Text
    end
end

function Library:_Keybind(flag, opts, parent, toggle, section, order, name)
    local kb = setmetatable({
        Type = "Keybind",
        Flag = flag,
        Key = ParseKey(opts.Default),
        Mode = opts.Mode == "Hold" and "Hold" or "Toggle",
        State = false,
        Callbacks = {},
        Toggle = toggle,
        Section = section,
        Name = opts.Text or name or flag,
        NoSave = opts.NoSave,
        NoTrigger = opts.NoTrigger,
        OnKey = opts.OnKey,
    }, Keybind)
    if opts.Callback then
        kb.Callbacks[1] = opts.Callback
    end
    kb.Button = Create("TextButton", {
        Size = UDim2.fromOffset(0, 16),
        AutomaticSize = Enum.AutomaticSize.X,
        TextSize = 10,
        LayoutOrder = order or 1,
        BackgroundTransparency = 0,
        Theme = { BackgroundColor3 = "Element", TextColor3 = "SubText" },
        Parent = parent,
    })
    Corner(kb.Button, 4)
    Stroke(kb.Button)
    Pad(kb.Button, 6, 0)
    Create("UISizeConstraint", { MinSize = Vector2.new(22, 16), Parent = kb.Button })
    kb.Button.Activated:Connect(function()
        kb:_Listen()
    end)
    kb.Button.MouseButton2Click:Connect(function()
        kb:_OpenMenu()
    end)
    kb:_Text()
    self.Options[flag] = kb
    self.Flags[flag] = false
    self.Keybinds[kb] = true
    self:UpdateHotkeys()
    if kb.OnKey then
        kb.OnKey(kb.Key)
    end
    return kb
end

function Section:AddKeybind(flag, opts)
    opts = opts or {}
    local text = opts.Text or flag
    local row, entry = self:_Row(20, text)
    Create("TextLabel", {
        Size = UDim2.new(1, -50, 1, 0),
        Text = text,
        Theme = { TextColor3 = "Text" },
        Parent = row,
    })
    local right = Create("Frame", {
        BackgroundTransparency = 1,
        AnchorPoint = Vector2.new(1, 0.5),
        Position = UDim2.new(1, 0, 0.5, 0),
        Size = UDim2.new(0, 0, 1, 0),
        AutomaticSize = Enum.AutomaticSize.X,
        Parent = row,
    })
    List(right, 6, true, Enum.HorizontalAlignment.Right)
    local kb = Library:_Keybind(flag, opts, right, nil, self, 1, text)
    kb.Entry = entry
    return kb
end

function ColorPicker:Set(color, alpha)
    if typeof(color) == "table" then
        color = Color3.new(color.R or color[1], color.G or color[2], color.B or color[3])
    end
    local h, s, v = color:ToHSV()
    self.H, self.S, self.V = h, s, v
    self.Base = { h, s, v }
    if alpha then
        self.Alpha = clamp(alpha, 0, 1)
    end
    self:_Update(true)
    self:Fire(self.Value, self.Alpha)
end

function ColorPicker:SetAnimation(mode)
    self.Animation = mode
    if mode == "None" then
        Library.Animated[self] = nil
        self.H, self.S, self.V = self.Base[1], self.Base[2], self.Base[3]
        self:_Update(false)
        self:Fire(self.Value, self.Alpha)
    else
        Library.Animated[self] = true
    end
    if self.AnimButtons then
        self:_AnimVisual()
    end
end

function ColorPicker:_AnimVisual()
    local theme = Library.Theme
    for mode, button in pairs(self.AnimButtons) do
        local active = self.Animation == mode
        button.BackgroundColor3 = active and theme.Accent or theme.Element
        button.TextColor3 = active and theme.OnAccent or theme.Text
    end
end

function ColorPicker:_Update(fromAnimation)
    local color = Color3.fromHSV(self.H, self.S, self.V)
    self.Value = color
    Library.Flags[self.Flag] = color
    self.Swatch.BackgroundColor3 = color
    self.Swatch.BackgroundTransparency = 0
    if self.Popup and self.Popup.Visible then
        self.SV.BackgroundColor3 = Color3.fromHSV(self.H, 1, 1)
        self.SVCursor.Position = UDim2.new(self.S, -4, 1 - self.V, -4)
        self.HueCursor.Position = UDim2.new(0, -2, self.H, -2)
        if self.HasAlpha then
            self.AlphaFill.BackgroundColor3 = color
            self.AlphaCursor.Position = UDim2.new(self.Alpha, -2, 0, -2)
        end
        if not self.Hex:IsFocused() then
            self.Hex.Text = "#" .. color:ToHex():upper()
        end
    end
    if fromAnimation == true and self.Animation ~= "None" then
        self:Fire(color, self.Alpha)
    end
end

function ColorPicker:_Manual()
    if self.Animation ~= "None" then
        self.Animation = "None"
        Library.Animated[self] = nil
        self:_AnimVisual()
    end
    self.Base = { self.H, self.S, self.V }
    self:_Update(false)
    self:Fire(self.Value, self.Alpha)
end

function ColorPicker:_Build()
    local lib = Library
    local width = 196
    local hasAlpha = self.HasAlpha
    local y = 8
    local height = 8 + 120 + 6 + (hasAlpha and 16 or 0) + 24 + 8 + 14 + 4 + 22 + 8
    local popup = Create("Frame", {
        Visible = false,
        Size = UDim2.fromOffset(width, height),
        Theme = { BackgroundColor3 = "Panel" },
        Parent = lib.Popups,
    })
    Corner(popup, 6)
    Stroke(popup)
    self.Popup = popup

    self.SV = Create("Frame", {
        Position = UDim2.fromOffset(8, y),
        Size = UDim2.fromOffset(156, 120),
        BackgroundColor3 = Color3.new(1, 0, 0),
        ClipsDescendants = true,
        Parent = popup,
    })
    Corner(self.SV, 4)
    local white = Create("Frame", {
        Size = UDim2.fromScale(1, 1),
        BackgroundColor3 = Color3.new(1, 1, 1),
        Parent = self.SV,
    })
    Create("UIGradient", {
        Transparency = NumberSequence.new({ NumberSequenceKeypoint.new(0, 0), NumberSequenceKeypoint.new(1, 1) }),
        Parent = white,
    })
    local black = Create("Frame", {
        Size = UDim2.fromScale(1, 1),
        BackgroundColor3 = Color3.new(0, 0, 0),
        Parent = self.SV,
    })
    Create("UIGradient", {
        Rotation = 90,
        Transparency = NumberSequence.new({ NumberSequenceKeypoint.new(0, 1), NumberSequenceKeypoint.new(1, 0) }),
        Parent = black,
    })
    self.SVCursor = Create("Frame", {
        Size = UDim2.fromOffset(8, 8),
        BackgroundTransparency = 1,
        ZIndex = 3,
        Parent = self.SV,
    })
    Corner(self.SVCursor, 4)
    Create("UIStroke", { Color = Color3.new(1, 1, 1), Thickness = 1.5, Parent = self.SVCursor })

    local hue = Create("Frame", {
        Position = UDim2.fromOffset(170, y),
        Size = UDim2.fromOffset(18, 120),
        BackgroundColor3 = Color3.new(1, 1, 1),
        Parent = popup,
    })
    Corner(hue, 4)
    local keys = {}
    for i = 0, 6 do
        keys[#keys + 1] = ColorSequenceKeypoint.new(i / 6, Color3.fromHSV(i / 6, 1, 1))
    end
    Create("UIGradient", { Rotation = 90, Color = ColorSequence.new(keys), Parent = hue })
    self.HueCursor = Create("Frame", {
        Size = UDim2.new(1, 4, 0, 4),
        BackgroundColor3 = Color3.new(1, 1, 1),
        ZIndex = 3,
        Parent = hue,
    })
    Corner(self.HueCursor, 2)
    Create("UIStroke", { Color = Color3.new(0, 0, 0), Thickness = 1, Parent = self.HueCursor })

    y = y + 120 + 6
    local alphaBar
    if hasAlpha then
        alphaBar = Create("Frame", {
            Position = UDim2.fromOffset(8, y),
            Size = UDim2.fromOffset(180, 10),
            BackgroundColor3 = rgb(70, 70, 70),
            ClipsDescendants = true,
            Parent = popup,
        })
        Corner(alphaBar, 4)
        self.AlphaFill = Create("Frame", {
            Size = UDim2.fromScale(1, 1),
            BackgroundColor3 = Color3.new(1, 1, 1),
            Parent = alphaBar,
        })
        Create("UIGradient", {
            Transparency = NumberSequence.new({ NumberSequenceKeypoint.new(0, 1), NumberSequenceKeypoint.new(1, 0) }),
            Parent = self.AlphaFill,
        })
        self.AlphaCursor = Create("Frame", {
            Size = UDim2.new(0, 4, 1, 4),
            BackgroundColor3 = Color3.new(1, 1, 1),
            ZIndex = 3,
            Parent = alphaBar,
        })
        Corner(self.AlphaCursor, 2)
        Create("UIStroke", { Color = Color3.new(0, 0, 0), Thickness = 1, Parent = self.AlphaCursor })
        y = y + 16
    end

    local hexHolder = Create("Frame", {
        Position = UDim2.fromOffset(8, y),
        Size = UDim2.fromOffset(180, 24),
        Theme = { BackgroundColor3 = "Element" },
        Parent = popup,
    })
    Corner(hexHolder, 5)
    Stroke(hexHolder)
    self.Hex = Create("TextBox", {
        Position = UDim2.fromOffset(9, 0),
        Size = UDim2.new(1, -18, 1, 0),
        Text = "",
        Theme = { TextColor3 = "Text" },
        Parent = hexHolder,
    })
    y = y + 24 + 8
    Create("TextLabel", {
        Position = UDim2.fromOffset(8, y),
        Size = UDim2.fromOffset(180, 14),
        Text = "Animations",
        TextSize = 11,
        Theme = { TextColor3 = "SubText" },
        Parent = popup,
    })
    y = y + 18
    local anims = Create("Frame", {
        BackgroundTransparency = 1,
        Position = UDim2.fromOffset(8, y),
        Size = UDim2.fromOffset(180, 22),
        Parent = popup,
    })
    List(anims, 4, true)
    self.AnimButtons = {}
    for _, mode in ipairs({ "None", "Rainbow", "Pulse" }) do
        local button = Create("TextButton", {
            Size = UDim2.new(1 / 3, -3, 1, 0),
            Text = mode,
            TextSize = 11,
            BackgroundTransparency = 0,
            Parent = anims,
        })
        Corner(button, 5)
        Stroke(button)
        button.Activated:Connect(function()
            self:SetAnimation(mode)
        end)
        self.AnimButtons[mode] = button
    end

    Track(self.SV, function(pt)
        local p, s = self.SV.AbsolutePosition, self.SV.AbsoluteSize
        self.S = clamp((pt.X - p.X) / s.X, 0, 1)
        self.V = 1 - clamp((pt.Y - p.Y) / s.Y, 0, 1)
        self:_Manual()
    end)
    Track(hue, function(pt)
        local p, s = hue.AbsolutePosition, hue.AbsoluteSize
        self.H = clamp((pt.Y - p.Y) / s.Y, 0, 0.999)
        self:_Manual()
    end)
    if alphaBar then
        Track(alphaBar, function(pt)
            local p, s = alphaBar.AbsolutePosition, alphaBar.AbsoluteSize
            self.Alpha = clamp((pt.X - p.X) / s.X, 0, 1)
            self:_Manual()
        end)
    end
    self.Hex.FocusLost:Connect(function()
        local ok, color = pcall(Color3.fromHex, self.Hex.Text)
        if ok and color then
            local h, s, v = color:ToHSV()
            self.H, self.S, self.V = h, s, v
            self:_Manual()
        else
            self:_Update(false)
        end
    end)
end

function ColorPicker:_Open()
    local lib = Library
    if self.Popup and lib:IsPopupOpen(self.Popup) then
        lib:ClosePopup()
        return
    end
    if not self.Popup then
        self:_Build()
    end
    lib:Place(self.Popup, self.Swatch, true)
    lib:OpenPopup(self.Popup, self.Swatch)
    self:_AnimVisual()
    self:_Update(false)
end

function Library:_ColorPicker(flag, opts, parent, section, order)
    local cp = setmetatable({
        Type = "ColorPicker",
        Flag = flag,
        Callbacks = {},
        Section = section,
        Alpha = opts.DefaultAlpha or 1,
        Animation = "None",
        HasAlpha = opts.Alpha ~= false,
        NoSave = opts.NoSave,
    }, ColorPicker)
    if opts.Callback then
        cp.Callbacks[1] = opts.Callback
    end
    cp.Swatch = Create("TextButton", {
        Size = UDim2.fromOffset(22, 12),
        LayoutOrder = order or 1,
        BackgroundTransparency = 0,
        Parent = parent,
    })
    Corner(cp.Swatch, 3)
    Stroke(cp.Swatch)
    cp.Swatch.Activated:Connect(function()
        cp:_Open()
    end)
    self.Options[flag] = cp
    local h, s, v = (opts.Default or Color3.new(1, 1, 1)):ToHSV()
    cp.H, cp.S, cp.V = h, s, v
    cp.Base = { h, s, v }
    cp:_Update(false)
    return cp
end

function Section:AddColorPicker(flag, opts)
    opts = opts or {}
    local text = opts.Text or flag
    local row, entry = self:_Row(20, text)
    Create("TextLabel", {
        Size = UDim2.new(1, -40, 1, 0),
        Text = text,
        Theme = { TextColor3 = "Text" },
        Parent = row,
    })
    local right = Create("Frame", {
        BackgroundTransparency = 1,
        AnchorPoint = Vector2.new(1, 0.5),
        Position = UDim2.new(1, 0, 0.5, 0),
        Size = UDim2.new(0, 0, 1, 0),
        AutomaticSize = Enum.AutomaticSize.X,
        Parent = row,
    })
    List(right, 6, true, Enum.HorizontalAlignment.Right)
    local cp = Library:_ColorPicker(flag, opts, right, self, 1)
    cp.Entry = entry
    return cp
end

function Window:AddSettings(opts)
    opts = opts or {}
    local lib = self.Library
    local tab = self:AddTab({ Name = opts.Name or "Settings", Icon = opts.Icon or "gear" })
    local menu = tab:AddSection({ Name = "Menu", Icon = "gear", Side = "Left" })
    menu:AddKeybind("UILib_MenuKey", {
        Text = "Menu Key",
        Default = self.MenuKey,
        NoTrigger = true,
        OnKey = function(key)
            self.MenuKey = key
        end,
    })
    menu:AddColorPicker("UILib_Accent", {
        Text = "Accent",
        Default = lib.Theme.Accent,
        Alpha = false,
        Callback = function(color)
            lib:SetAccent(color)
        end,
    })
    menu:AddToggle("UILib_Hotkeys", {
        Text = "Hotkey List",
        Default = true,
        Callback = function(state)
            lib:SetHotkeysVisible(state)
        end,
    })
    menu:AddToggle("UILib_Watermark", {
        Text = "Watermark",
        Default = lib.Watermark ~= nil and lib.Watermark.Visible,
        Callback = function(state)
            if lib.Watermark then
                lib.Watermark.Visible = state
            end
        end,
    })
    menu:AddButton({
        Text = "Unload",
        Confirm = true,
        Callback = function()
            lib:Unload()
        end,
    })
    local cfg = tab:AddSection({ Name = "Configs", Icon = "file", Side = "Right" })
    local name = cfg:AddInput("UILib_CfgName", { Text = "Config Name", Placeholder = "default", NoSave = true })
    local picker = cfg:AddDropdown("UILib_CfgList", {
        Text = "Saved Configs",
        Values = lib:GetConfigs(),
        AllowNull = true,
        NoSave = true,
        Callback = function(value)
            if value then
                name:Set(value, true)
            end
        end,
    })
    local function refresh()
        picker:SetValues(lib:GetConfigs())
    end
    local function target()
        local value = name.Value
        if value == "" then
            value = picker.Value or ""
        end
        return value
    end
    cfg:AddButton({
        Text = "Save",
        Callback = function()
            local ok, err = lib:SaveConfig(target())
            lib:Notify({ Title = "Config", Text = ok and ("Saved " .. target()) or tostring(err) })
            refresh()
        end,
    })
    cfg:AddButton({
        Text = "Load",
        Callback = function()
            local ok, err = lib:LoadConfig(target())
            lib:Notify({ Title = "Config", Text = ok and ("Loaded " .. target()) or tostring(err) })
        end,
    })
    cfg:AddButton({
        Text = "Delete",
        Confirm = true,
        Callback = function()
            local ok, err = lib:DeleteConfig(target())
            lib:Notify({ Title = "Config", Text = ok and ("Deleted " .. target()) or tostring(err) })
            refresh()
        end,
    })
    cfg:AddButton({ Text = "Refresh", Callback = refresh })
    return tab
end

Env.UILibrary = Library

return Library
