namespace Vista.SDK;

public sealed record GmodPathQuery
{
    internal readonly GmodPathQueryBuilder Builder;

    internal GmodPathQuery(GmodPathQueryBuilder builder) => Builder = builder;

    public bool Match(GmodPath? other) => Builder.Match(other);
}

public abstract record GmodPathQueryBuilder
{
    private GmodPathQueryBuilder()
    {
        _filter = new();
    }

    private readonly Dictionary<string, NodeItem> _filter;

    public static Nodes Empty() => new Nodes();

    public static Path From(GmodPath path) => new(path);

    public GmodPathQuery Build() => new(this);

    public sealed record Path : GmodPathQueryBuilder
    {
        public readonly GmodPath GmodPath;
        private Dictionary<string, GmodNode> _setNodes;

        internal Path(GmodPath path)
        {
            _setNodes = new();

            foreach (var set in path.IndividualizableSets)
            {
                var setNode = set.Nodes[set.Nodes.Count - 1];
                _setNodes.Add(setNode.Code, setNode);
                HashSet<Location> locations = [];
                if (set.Location is not null)
                    locations.Add(set.Location.Value);
                _filter.Add(setNode.Code, new(setNode, locations));
            }

            GmodPath = path;
        }

        public Path WithNode(
            Func<IReadOnlyDictionary<string, GmodNode>, GmodNode> select,
            bool matchAllLocations = false
        )
        {
            var node = select(_setNodes);
            if (!_filter.TryGetValue(node.Code, out var item))
                throw new Exception("Expected to find a filter on the node in the path");
            item.Locations = new();
            item.MatchAllLocations = matchAllLocations;
            return this with { };
        }

        public Path WithNode(Func<IReadOnlyDictionary<string, GmodNode>, GmodNode> select, params Location[]? locations)
        {
            var node = select(_setNodes);
            if (!_filter.TryGetValue(node.Code, out var item))
                throw new Exception("Expected to find a filter on the node in the path");
            item.Locations = locations is null ? new HashSet<Location>() : new(locations);
            return this with { };
        }

        public Path WithoutLocations()
        {
            foreach (var item in _filter.Values)
            {
                item.Locations = new();
                item.MatchAllLocations = true;
            }
            return this with { };
        }
    }

    public sealed record Nodes : GmodPathQueryBuilder
    {
        internal Nodes() { }

        public Nodes WithNode(GmodNode node, bool MatchAllLocations = false)
        {
            if (_filter.TryGetValue(node.Code, out var item))
            {
                item.Locations = new();
                item.MatchAllLocations = MatchAllLocations;
            }
            else
            {
                _filter.Add(node.Code, new(node, new()) { MatchAllLocations = MatchAllLocations });
            }
            return this with { };
        }

        public Nodes WithNode(GmodNode node, params Location[]? locations)
        {
            var newLocations = locations is null ? new HashSet<Location>() : new(locations);
            if (_filter.TryGetValue(node.Code, out var item))
            {
                item.Locations = newLocations;
            }
            else
            {
                _filter.Add(node.Code, new(node, newLocations));
            }
            return this with { };
        }
    }

    private static GmodPath EnsurePathVersion(GmodPath path)
    {
        GmodPath p = path;
        if (p.VisVersion < VIS.LatestVisVersion)
        {
            var convertedPath =
                VIS.Instance.ConvertPath(p, VIS.LatestVisVersion) ?? throw new Exception("Failed to convert path");
            p = convertedPath;
        }
        return p;
    }

    private static GmodNode EnsureNodeVersion(GmodNode node)
    {
        GmodNode n = node;
        if (n.VisVersion < VIS.LatestVisVersion)
        {
            var convertedNode =
                VIS.Instance.ConvertNode(n, VIS.LatestVisVersion) ?? throw new Exception("Failed to convert node");
            n = convertedNode;
        }
        return n;
    }

    internal bool Match(GmodPath? other)
    {
        if (other is null)
            return false;
        var target = EnsurePathVersion(other);

        Dictionary<string, List<Location>> targetNodes = new();
        var nodes = target.Parents.Concat([target.Node]);
        foreach (var node in nodes)
        {
            if (!targetNodes.TryGetValue(node.Code, out var locations))
                targetNodes.Add(node.Code, locations = new());
            if (node.Location is not null)
                locations.Add(node.Location.Value);
        }

        foreach (var kvp in _filter)
        {
            var item = kvp.Value;
            var node = EnsureNodeVersion(item.Node);

            if (!targetNodes.TryGetValue(node.Code, out var potentialLocations))
                return false;
            if (item.MatchAllLocations)
                continue;
            if (item.Locations.Count > 0)
            {
                if (potentialLocations.Count == 0)
                    return false;
                if (!potentialLocations.Any(item.Locations.Contains))
                    return false;
            }
            else
            {
                if (potentialLocations.Count > 0)
                    return false;
            }
        }

        return true;
    }
}

internal sealed record NodeItem
{
    public NodeItem(GmodNode node, HashSet<Location> locations)
    {
        Node = node;
        Locations = locations;
    }

    public GmodNode Node { get; set; }
    public HashSet<Location> Locations { get; set; }

    public bool MatchAllLocations { get; set; }
}
