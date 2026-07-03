import torch
import torch.nn as nn
import csv
from sklearn.datasets import make_moons
import numpy as np

def train_model(X, y, hidden=8, epochs=500, lr=0.01):
    n_classes = len(torch.unique(y))
    model = nn.Sequential(
        nn.Linear(2, hidden),
        nn.ReLU(),
        nn.Linear(hidden, n_classes)
    )
    optimizer = torch.optim.Adam(model.parameters(), lr=lr)
    loss_fn = nn.CrossEntropyLoss()
    for _ in range(epochs):
        optimizer.zero_grad()
        loss = loss_fn(model(X), y)
        loss.backward()
        optimizer.step()
    print(f"Final loss: {loss.item():.4f}")
    return model

def save_model(model, name):
    scripted = torch.jit.script(model)
    scripted.save(f"../models/{name}.pt")
    print(f"Model saved: {name}.pt")

def save_data(X, y, name):
    with open(f"../data/{name}.csv", 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['x', 'y', 'label'])
        for i in range(len(X)):
            writer.writerow([X[i][0].item(), X[i][1].item(), y[i].item()])
    print(f"Data saved: {name}.csv")

def generate_preset(name, X, y):
    print(f"\n--- Generating preset: {name} ---")
    model = train_model(X, y)
    save_model(model, name)
    save_data(X, y, name)

def preset_separated():
    torch.manual_seed(42)
    X0 = torch.randn(100, 2) + torch.tensor([-2.0, -2.0])
    X1 = torch.randn(100, 2) + torch.tensor([2.0, 2.0])
    X = torch.cat([X0, X1])
    y = torch.cat([torch.zeros(100), torch.ones(100)]).long()
    generate_preset("separated", X, y)

def preset_overlapping():
    torch.manual_seed(42)
    X0 = torch.randn(100, 2) + torch.tensor([-1.0, -1.0])
    X1 = torch.randn(100, 2) + torch.tensor([1.0, 1.0])
    X = torch.cat([X0, X1])
    y = torch.cat([torch.zeros(100), torch.ones(100)]).long()
    generate_preset("overlapping", X, y)

def preset_moons():    
    X_np, y_np = make_moons(n_samples=200, noise=0.2, random_state=42)
    
    # Scale to fit our data space [-6, 6]
    X_np = X_np * 2.5
    
    X = torch.tensor(X_np, dtype=torch.float32)
    y = torch.tensor(y_np, dtype=torch.long)
    generate_preset("moons", X, y)

def preset_three_classes():
    torch.manual_seed(42)
    X0 = torch.randn(100, 2) + torch.tensor([-2.0, -2.0])
    X1 = torch.randn(100, 2) + torch.tensor([2.0, -2.0])
    X2 = torch.randn(100, 2) + torch.tensor([0.0, 2.0])
    X = torch.cat([X0, X1, X2])
    y = torch.cat([torch.zeros(100), torch.ones(100), torch.full((100,), 2)]).long()
    generate_preset("three_classes", X, y)

if __name__ == "__main__":
    preset_separated()
    preset_overlapping()
    preset_moons()
    preset_three_classes()
