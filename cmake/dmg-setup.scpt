-- DMG Setup Script for SynthFlow
-- This script configures the appearance of the DMG installer

tell application "Finder"
    tell disk "SynthFlow"
        open

        -- Set view options
        set current view of container window to icon view
        set toolbar visible of container window to false
        set statusbar visible of container window to false

        -- Set window size and position
        set the bounds of container window to {400, 100, 1000, 550}

        -- Set icon view options
        set theViewOptions to the icon view options of container window
        set arrangement of theViewOptions to not arranged
        set icon size of theViewOptions to 80

        -- Position the application icon
        set position of item "SynthFlow.app" of container window to {130, 200}

        -- Position the Applications link
        set position of item "Applications" of container window to {470, 200}

        -- Close and reopen to apply changes
        close container window
        open container window

        -- Update window bounds again
        set the bounds of container window to {400, 100, 1000, 550}
    end tell
end tell