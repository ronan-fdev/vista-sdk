import { GmodNode } from "./GmodNode";
import { GmodPath } from "./GmodPath";
import { Locations } from "./Location";
import {
    GmodTuple,
    TraversalHandler,
    TraversalHandlerWithState,
    TraversalContext,
    TraversalHandlerResult,
    TraversalOptions,
} from "./types/Gmod";
import { GmodDto } from "./types/GmodDto";
import { GmodNodeMetadata } from "./types/GmodNode";
import { Parents } from "./util/Parents";
import { naturalSort } from "./util/util";
import { VisVersion } from "./VisVersion";

export const PotentialParentScopeTypes = ["SELECTION", "GROUP", "LEAF"];

export class Gmod {
    public visVersion: VisVersion;
    private _rootNode: GmodNode;
    private _nodeMap: Map<string, GmodNode>;

    public constructor(visVersion: VisVersion, dto: GmodDto) {
        this.visVersion = visVersion;

        this._nodeMap = new Map<string, GmodNode>();
        let rootNodeId: string | undefined = undefined;
        for (const nodeDto of dto.items) {
            const node = GmodNode.createFromDto(visVersion, nodeDto);
            if (node.code === "VE") {
                rootNodeId = node.id;
            }

            this._nodeMap.set(nodeDto.id ?? nodeDto.code, node);
        }

        dto.relations.sort(([_, a], [__, b]) => naturalSort(a, b));
        for (const relation of dto.relations) {
            const parentCode = relation[0];
            const childCode = relation[1];

            const parentNode = this._nodeMap.get(parentCode);
            const childNode = this._nodeMap.get(childCode);
            if (!parentNode)
                throw new Error(
                    "Couldnt find parent node with code: " + parentCode
                );
            if (!childNode)
                throw new Error(
                    "Couldnt find child node with code: " + parentCode
                );

            parentNode.addChild(childNode);
            childNode.addParent(parentNode);
        }

        if (!rootNodeId) throw new Error("Couldnt find root node");
        const rootNode = this._nodeMap.get(rootNodeId);
        if (!rootNode) throw new Error("Couldnt find root node");

        this._rootNode = rootNode;
    }

    public get rootNode() {
        return this._rootNode;
    }

    public static get leafTypes(): GmodTuple[] {
        return [
            { category: "ASSET FUNCTION", type: "LEAF" },
            { category: "PRODUCT FUNCTION", type: "LEAF" },
            { category: "PRODUCT", type: "GROUP LEVEL 2" },
        ];
    }

    public static isLeafNode(values: GmodTuple | GmodNodeMetadata): boolean {
        const { category, type } = values;
        for (const leafType of this.leafTypes) {
            if (leafType.category === category && leafType.type === type)
                return true;
        }

        return false;
    }

    public static isFunctionNode(category: string | GmodNodeMetadata): boolean {
        const c = typeof category === "string" ? category : category.category;
        return c !== "PRODUCT" && c !== "ASSET";
    }

    public static isProductSelection(metadata: GmodNodeMetadata) {
        return (
            metadata.category.includes("PRODUCT") &&
            metadata.type.includes("SELECTION")
        );
    }

    public static isProductType(metadata: GmodNodeMetadata) {
        return metadata.category === "PRODUCT" && metadata.type === "TYPE";
    }

    public static isAsset(metadata: GmodNodeMetadata) {
        return metadata.category == "ASSET";
    }

    public static isProductTypeAssignment(parent?: GmodNode, child?: GmodNode) {
        if (!parent || !child) return false;
        if (!parent.metadata.category.includes("FUNCTION")) return false;
        if (
            child.metadata.category !== "PRODUCT" ||
            child.metadata.type !== "TYPE"
        )
            return false;
        return true;
    }

    public static isProductSelectionAssignment(
        parent?: GmodNode,
        child?: GmodNode
    ) {
        if (!parent || !child) return false;
        if (!parent.metadata.category.includes("FUNCTION")) return false;
        if (
            !child.metadata.category.includes("PRODUCT") ||
            child.metadata.type !== "SELECTION"
        )
            return false;
        return true;
    }

    public getNode(key: string): GmodNode {
        const node = this.tryGetNode(key);

        if (!node) {
            throw new Error("Couldn't get GmodNode");
        }

        return node;
    }

    public tryGetNode(key: string, location?: string): GmodNode | undefined {
        const node = this._nodeMap.get(key);
        if (!node) return;
        return node;
    }

    // Parsing
    public parsePath(item: string, locations: Locations): GmodPath {
        const path = this.tryParsePath(item, locations);

        if (!path) {
            throw new Error("Couldn't parse GmodPath");
        }
        return path;
    }

    public tryParsePath(
        item: string,
        locations: Locations
    ): GmodPath | undefined {
        return GmodPath.tryParse(item, locations, this);
    }

    // Parsing
    public parseFromFullPath(item: string, locations: Locations): GmodPath {
        return GmodPath.parseFromFullPath(item, this, locations);
    }

    public tryParseFromFullPath(
        item: string,
        locations: Locations
    ): GmodPath | undefined {
        return GmodPath.tryParseFromFullPath(item, this, locations);
    }

    // Traversal
    public traverse<T>(
        handler: TraversalHandler | TraversalHandlerWithState<T>,
        params?: TraversalOptions<T>
    ): boolean {
        const {
            rootNode = this._rootNode,
            state,
            maxTraversalOccurrence,
        } = params ?? {};

        if (!state) {
            return this.traverseFromNodeWithState(
                handler as TraversalHandler,
                rootNode,
                (parents, node, handler) => handler(parents, node)
            );
        }

        return this.traverseFromNodeWithState(
            state,
            rootNode,
            handler as TraversalHandlerWithState<T>,
            { maxTraversalOccurrence }
        );
    }

    public traverseFromNodeWithState<T>(
        state: T,
        rootNode: GmodNode,
        handler: TraversalHandlerWithState<T>,
        options?: Pick<TraversalOptions<T>, "maxTraversalOccurrence">
    ): boolean {
        const context: TraversalContext<T> = {
            parents: new Parents(),
            handler,
            state,
            maxTraversalOccurrence: options?.maxTraversalOccurrence ?? 1,
        };
        return (
            this.traverseNode<T>(context, rootNode) ===
            TraversalHandlerResult.Continue
        );
    }

    private traverseNode<T>(
        context: TraversalContext<T>,
        node: GmodNode
    ): TraversalHandlerResult {
        if (node.metadata.installSubstructure === false)
            return TraversalHandlerResult.Continue;

        let result = context.handler(
            context.parents.asList,
            node,
            context.state
        );

        if (
            result === TraversalHandlerResult.Stop ||
            result === TraversalHandlerResult.SkipSubtree
        )
            return result;

        var skipOccurenceCheck = Gmod.isProductSelectionAssignment(
            context.parents.last(),
            node
        );
        // Skip the occurence check for "hidden" nodes such as selections, etc.
        if (!skipOccurenceCheck) {
            var occ = context.parents.occurrences(node);
            if (occ == context.maxTraversalOccurrence)
                return TraversalHandlerResult.SkipSubtree;
            if (occ > context.maxTraversalOccurrence)
                throw new Error(
                    "Invalid state - node occured more than expected"
                );
        }

        context.parents.push(node);

        for (const child of node.children) {
            result = this.traverseNode(context, child);
            if (result === TraversalHandlerResult.Stop) return result;
            else if (result === TraversalHandlerResult.SkipSubtree) continue;
        }

        context.parents.pop();
        return TraversalHandlerResult.Continue;
    }

    public *[Symbol.iterator](): Generator<GmodNode> {
        for (const [_, value] of this._nodeMap) yield value;
    }
}
