import sys

def distance(point1, point2):
    return sum((a - b) ** 2 for a, b in zip(point1, point2)) ** 0.5

def assign_to_clusters(data, centroids):
    clusters = [[] for _ in centroids]
    for point in data:
        distances = [distance(point, centroid) for centroid in centroids]
        cluster_index = distances.index(min(distances))
        clusters[cluster_index].append(point)
    return clusters

def update_centroids(clusters):
    return [list(map(lambda x: sum(x) / len(cluster), zip(*cluster))) for cluster in clusters]

def kmeans(data, centroids, iter):
    for _ in range(iter):
        clusters = assign_to_clusters(data, centroids)
        
        new_centroids = update_centroids(clusters)
        
        if all(distance(c, nc) < 0.001 for c, nc in zip(centroids, new_centroids)):
            break
        
        centroids = new_centroids

    return centroids


def kmeans_clustering(data, k, iter):
    centroids = data[:k]
    centroids = kmeans(data, centroids, iter)

    clusters = []
    for x in data:
        distances = [distance(x, c) for c in centroids]
        clusters.append(distances.index(min(distances)))

    return clusters
