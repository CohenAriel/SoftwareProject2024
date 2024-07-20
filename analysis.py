import sys
import numpy as np
from kmeans import kmeans_clustering
from sklearn.metrics import silhouette_score
import symnmfmodule

np.random.seed(0)

# Print the silhouette score
def print_silhouette_score(label, score):
    print(f'{label}: {score:.4f}')

# Perform symNMF clustering
def symnmf_clustering(X, k):
    W = symnmfmodule.norm(X)
    H = np.random.uniform(0, 2 * (np.mean(W)/k)**0.5, (len(X), k)).tolist()
    H = symnmfmodule.symnmf(H, W)
    clusters = np.argmax(H, axis=1)
    
    return clusters

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 analysis.py <k> <input_file>")
        sys.exit(1)

    k = int(sys.argv[1])
    file_name = sys.argv[2]

    # Load the data in the file
    X = np.loadtxt(file_name, delimiter=',', ndmin=2).tolist()

    # Perform symNMF clustering
    symnmf_clusters = symnmf_clustering(X, k)
    symnmf_score = silhouette_score(X, symnmf_clusters)

    # Perform k-means clustering
    kmeans_clusters = kmeans_clustering(X, k, 300)
    kmeans_score = silhouette_score(X, kmeans_clusters)

    # Print the silhouette scores
    print_silhouette_score('nmf', symnmf_score)
    print_silhouette_score('kmeans', kmeans_score)