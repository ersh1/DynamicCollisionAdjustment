# USER DEFINED
$outDir = "C:\Skyrim Mod Organizer\mods\Dynamic Collision Adjustment\Interface\Translations"

$strings = @('czech', 'english', 'french', 'german', 'italian', 'japanese', 'polish', 'russian', 'spanish')

ForEach ($string in $strings)
{
    Copy-Item "DynamicCollisionAdjustment_english.txt" -Destination "$outDir\DynamicCollisionAdjustment_$string.txt"
}