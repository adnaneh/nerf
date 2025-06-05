import networkx as nx
import matplotlib.pyplot as plt
import numpy as np

# The confusion matrix first half from hireme.c
confusion_first_half = [
    0xac,0xd1,0x25,0x94,0x1f,0xb3,0x33,0x28,0x7c,0x2b,0x17,0xbc,0xf6,0xb0,0x55,0x5d,
    0x8f,0xd2,0x48,0xd4,0xd3,0x78,0x62,0x1a,0x02,0xf2,0x01,0xc9,0xaa,0xf0,0x83,0x71,
    0x72,0x4b,0x6a,0xe8,0xe9,0x42,0xc0,0x53,0x63,0x66,0x13,0x4a,0xc1,0x85,0xcf,0x0c,
    0x24,0x76,0xa5,0x6e,0xd7,0xa1,0xec,0xc6,0x04,0xc2,0xa2,0x5c,0x81,0x92,0x6c,0xda,
    0xc6,0x86,0xba,0x4d,0x39,0xa0,0x0e,0x8c,0x8a,0xd0,0xfe,0x59,0x96,0x49,0xe6,0xea,
    0x69,0x30,0x52,0x1c,0xe0,0xb2,0x05,0x9b,0x10,0x03,0xa8,0x64,0x51,0x97,0x02,0x09,
    0x8e,0xad,0xf7,0x36,0x47,0xab,0xce,0x7f,0x56,0xca,0x00,0xe3,0xed,0xf1,0x38,0xd8,
    0x26,0x1c,0xdc,0x35,0x91,0x43,0x2c,0x74,0xb4,0x61,0x9d,0x5e,0xe9,0x4c,0xbf,0x77,
    0x16,0x1e,0x21,0x1d,0x2d,0xa9,0x95,0xb8,0xc3,0x8d,0xf8,0xdb,0x34,0xe1,0x84,0xd6,
    0x0b,0x23,0x4e,0xff,0x3c,0x54,0xa7,0x78,0xa4,0x89,0x33,0x6d,0xfb,0x79,0x27,0xc4,
    0xf9,0x40,0x41,0xdf,0xc5,0x82,0x93,0xdd,0xa6,0xef,0xcd,0x8d,0xa3,0xae,0x7a,0xb6,
    0x2f,0xfd,0xbd,0xe5,0x98,0x66,0xf3,0x4f,0x57,0x88,0x90,0x9c,0x0a,0x50,0xe7,0x15,
    0x7b,0x58,0xbc,0x07,0x68,0x3a,0x5f,0xee,0x32,0x9f,0xeb,0xcc,0x18,0x8b,0xe2,0x57,
    0xb7,0x49,0x37,0xde,0xf5,0x99,0x67,0x5b,0x3b,0xbb,0x3d,0xb5,0x2d,0x19,0x2e,0x0d,
    0x93,0xfc,0x7e,0x06,0x08,0xbe,0x3f,0xd9,0x2a,0x70,0x9a,0xc8,0x7d,0xd8,0x46,0x65,
    0x22,0xf4,0xb9,0xa2,0x6f,0x12,0x1b,0x14,0x45,0xc7,0x87,0x31,0x60,0x29,0xf7,0x73
]

# Find all unique values in the first half
present_values = list(set(confusion_first_half))
present_values.sort()

# Find missing values (0-255 that are not in confusion_first_half)
all_values = set(range(256))
present_set = set(confusion_first_half)
missing_values = list(all_values - present_set)
missing_values.sort()

print(f"Number of unique present values: {len(present_values)}")
print(f"Number of missing values: {len(missing_values)}")
print(f"Missing values: {[hex(v) for v in missing_values]}")

# Create graph
G = nx.Graph()

# Add all present values as nodes
for val in present_values:
    G.add_node(val)

# Add edges between nodes whose XOR is in the missing values set
edge_count = 0
for i, val1 in enumerate(present_values):
    for val2 in present_values[i+1:]:  # Avoid duplicate edges
        xor_result = val1 ^ val2
        if xor_result in missing_values:
            G.add_edge(val1, val2, missing_value=xor_result)
            edge_count += 1

print(f"\nNumber of edges (XOR relationships producing missing values): {edge_count}")

# Analyze graph structure
print(f"Number of connected components: {nx.number_connected_components(G)}")
print(f"Number of isolated nodes: {len(list(nx.isolates(G)))}")

# Find nodes with highest degree (most XOR relationships)
degrees = dict(G.degree())
sorted_degrees = sorted(degrees.items(), key=lambda x: x[1], reverse=True)
print(f"\nTop 10 nodes by degree (number of XOR relationships):")
for node, degree in sorted_degrees[:10]:
    print(f"  Node 0x{node:02x}: {degree} edges")

# Create visualization
plt.figure(figsize=(20, 16))

# Use spring layout for better visualization
pos = nx.spring_layout(G, k=3, iterations=50, seed=42)

# Draw nodes
node_sizes = [300 + 50 * degrees[node] for node in G.nodes()]
nx.draw_networkx_nodes(G, pos, node_size=node_sizes, node_color='lightblue', alpha=0.7)

# Draw edges with different colors based on the missing value they produce
edge_colors = plt.cm.rainbow(np.linspace(0, 1, len(missing_values)))
missing_to_color = {val: edge_colors[i] for i, val in enumerate(missing_values)}

for edge in G.edges(data=True):
    missing_val = edge[2]['missing_value']
    nx.draw_networkx_edges(G, pos, [(edge[0], edge[1])], 
                          edge_color=[missing_to_color[missing_val]], 
                          alpha=0.5, width=2)

# Draw labels for nodes
labels = {node: f'0x{node:02x}' for node in G.nodes()}
nx.draw_networkx_labels(G, pos, labels, font_size=8)

plt.title("XOR Graph: Edges connect values whose XOR produces a missing value", fontsize=16)
plt.axis('off')
plt.tight_layout()
plt.savefig('/Users/adnanehamid/nerf/xor_graph_full.png', dpi=300, bbox_inches='tight')
plt.close()

# Create a focused visualization on the most connected component
if nx.number_connected_components(G) > 1:
    # Get the largest connected component
    largest_cc = max(nx.connected_components(G), key=len)
    G_sub = G.subgraph(largest_cc).copy()
    
    plt.figure(figsize=(16, 12))
    pos_sub = nx.spring_layout(G_sub, k=2, iterations=50, seed=42)
    
    # Draw the subgraph
    node_sizes_sub = [300 + 50 * G_sub.degree(node) for node in G_sub.nodes()]
    nx.draw_networkx_nodes(G_sub, pos_sub, node_size=node_sizes_sub, node_color='lightcoral', alpha=0.7)
    
    for edge in G_sub.edges(data=True):
        missing_val = edge[2]['missing_value']
        nx.draw_networkx_edges(G_sub, pos_sub, [(edge[0], edge[1])], 
                              edge_color=[missing_to_color[missing_val]], 
                              alpha=0.6, width=2)
    
    labels_sub = {node: f'0x{node:02x}' for node in G_sub.nodes()}
    nx.draw_networkx_labels(G_sub, pos_sub, labels_sub, font_size=10)
    
    plt.title(f"Largest Connected Component ({len(G_sub)} nodes)", fontsize=16)
    plt.axis('off')
    plt.tight_layout()
    plt.savefig('/Users/adnanehamid/nerf/xor_graph_largest_component.png', dpi=300, bbox_inches='tight')
    plt.close()

# Create a detailed analysis report
with open('/Users/adnanehamid/nerf/xor_graph_analysis.txt', 'w') as f:
    f.write("XOR Graph Analysis Report\n")
    f.write("========================\n\n")
    
    f.write(f"Total present values: {len(present_values)}\n")
    f.write(f"Total missing values: {len(missing_values)}\n")
    f.write(f"Missing values: {[hex(v) for v in missing_values]}\n\n")
    
    f.write(f"Graph Statistics:\n")
    f.write(f"- Total nodes: {G.number_of_nodes()}\n")
    f.write(f"- Total edges: {G.number_of_edges()}\n")
    f.write(f"- Connected components: {nx.number_connected_components(G)}\n")
    f.write(f"- Isolated nodes: {len(list(nx.isolates(G)))}\n\n")
    
    f.write("All XOR relationships producing missing values:\n")
    for edge in G.edges(data=True):
        val1, val2 = edge[0], edge[1]
        missing = edge[2]['missing_value']
        f.write(f"0x{val1:02x} ⊕ 0x{val2:02x} = 0x{missing:02x}\n")
    
    f.write(f"\nDegree distribution:\n")
    degree_dist = {}
    for node in G.nodes():
        deg = G.degree(node)
        degree_dist[deg] = degree_dist.get(deg, 0) + 1
    
    for deg in sorted(degree_dist.keys()):
        f.write(f"  Degree {deg}: {degree_dist[deg]} nodes\n")

print("\nVisualization complete! Generated files:")
print("- xor_graph_full.png: Full graph visualization")
print("- xor_graph_largest_component.png: Largest connected component")
print("- xor_graph_analysis.txt: Detailed analysis report")