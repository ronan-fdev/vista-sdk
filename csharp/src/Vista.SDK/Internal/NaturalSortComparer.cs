namespace Vista.SDK;

internal class NaturalSortComparer : IComparer<string>
{
    public static readonly IComparer<string> Instance = new NaturalSortComparer();

    public int Compare(string? a, string? b)
    {
        if (a == null && b == null)
            return 0;
        if (a == null)
            return -1;
        if (b == null)
            return 1;
        int aIndex = 0,
            bIndex = 0;

        while (aIndex < a.Length && bIndex < b.Length)
        {
            if (char.IsDigit(a[aIndex]) && char.IsDigit(b[bIndex]))
            {
                // Extract numeric parts
                int aNumStart = aIndex;
                while (aIndex < a.Length && char.IsDigit(a[aIndex]))
                    aIndex++;
                int aNum = int.Parse(a.Substring(aNumStart, aIndex - aNumStart));

                int bNumStart = bIndex;
                while (bIndex < b.Length && char.IsDigit(b[bIndex]))
                    bIndex++;
                int bNum = int.Parse(b.Substring(bNumStart, bIndex - bNumStart));

                if (aNum != bNum)
                    return aNum.CompareTo(bNum);
            }
            else
            {
                // Compare characters normally
                if (aIndex < a.Length && bIndex < b.Length && a[aIndex] != b[bIndex])
                    return a[aIndex].CompareTo(b[bIndex]);

                if (aIndex < a.Length)
                    aIndex++;
                if (bIndex < b.Length)
                    bIndex++;
            }
        }

        return a.Length.CompareTo(b.Length);
    }
}
