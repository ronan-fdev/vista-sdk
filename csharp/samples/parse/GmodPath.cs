using Vista.SDK;

/// <summary>
/// Converts a GmodPath from one version to another.
/// Working with ISO19848, the <see cref="LocalId.Parse(string)"/> is usually sufficient.
/// See <see cref="ParseLocalId"/> for example usage.
/// </summary>
static class ParseGmodPath
{
    public static GmodPath? Parse(string gmodPath, VisVersion visVersion)
    {
        // Simply use the static Parse method
        var path = GmodPath.Parse(gmodPath, visVersion);
        return path;
    }

    public static GmodPath? ParseFullPath(string fullPath, VisVersion visVersion)
    {
        // Simply use the static Parse method
        var path = GmodPath.ParseFullPath(fullPath, visVersion);
        return path;
    }

    public static GmodPath? TryParse(string path, VisVersion visVersion)
    {
        // Use the TryParse method
        if (GmodPath.TryParse(path, visVersion, out var gmodPath))
        {
            // Handle result as you please
            return gmodPath;
        }
        return null;
    }

    public static GmodPath? TryParseFullPath(string fullPath, VisVersion visVersion)
    {
        // Use the TryParseFullPath method
        if (GmodPath.TryParseFullPath(fullPath, visVersion, out var gmodPath))
        {
            // Handle result as you please
            return gmodPath;
        }
        return null;
    }
}
