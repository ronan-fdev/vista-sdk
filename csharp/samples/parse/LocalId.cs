using Vista.SDK;

static class ParseLocalId
{
    public static LocalId? Parse(string localId)
    {
        // Simply use the static Parse method
        var id = LocalId.Parse(localId);
        return id;
    }

    public static LocalId? TryParse(string localId)
    {
        // Use the TryParse method
        if (LocalId.TryParse(localId, out var errors, out var id))
        {
            // Handle success as you please
            return id;
        }
        // Handle errors as you please
        return null;
    }
}
