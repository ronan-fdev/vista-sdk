using Vista.SDK;

static class ParseMetadataTag
{
    public static MetadataTag? Parse(string tag, CodebookName codebook, VisVersion visVersion)
    {
        // Need to create a metadata tag in scope of a codebook
        var codebooks = VIS.Instance.GetCodebooks(visVersion);
        // For now, use this for nullable result. Will be replaced by proper Try pattern.
        var metadataTag = codebooks.TryCreateTag(codebook, tag);
        if (metadataTag is null)
        {
            // Handle the case where the metadata tag could not be created
            return null;
        }
        return metadataTag;
    }
}
