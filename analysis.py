import sys
import numpy as np
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_score
import symnmfmodule

def print_silhouette_score(label, score):
    print(f'{label}: {score:.4f}')

def load_data(file_name):
    return np.loadtxt(file_name, delimiter=',')

def symnmf_clustering(X, k):
    W = symnmfmodule.norm(X.tolist())
    m = np.mean(W)
    H = np.random.uniform(0, 2 * (m / k)**0.5, (len(X), k))
    H = symnmfmodule.symnmf(W, H.tolist())
    clusters = np.argmax(H, axis=1)
    return clusters

def kmeans_clustering(X, k):
    kmeans = KMeans(n_clusters=k, random_state=0).fit(X)
    clusters = kmeans.labels_
    return clusters

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 analysis.py <k> <input_file>")
        sys.exit(1)

    k = int(sys.argv[1])
    file_name = sys.argv[2]

    X = load_data(file_name)

    symnmf_clusters = symnmf_clustering(X, k)
    symnmf_score = silhouette_score(X, symnmf_clusters)

    kmeans_clusters = kmeans_clustering(X, k)
    kmeans_score = silhouette_score(X, kmeans_clusters)

    print_silhouette_score('nmf', symnmf_score)
    print_silhouette_score('kmeans', kmeans_score)