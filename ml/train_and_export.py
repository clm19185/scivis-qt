import torch
import torch.nn as nn
import csv

# --- Synthetic dataset: two well-separated 2D Gaussian clusters ---
torch.manual_seed(42) # for reproducibility
X0 = torch.randn(100, 2) + torch.tensor([-2.0, -2.0])  # classe 0 — bottom left
X1 = torch.randn(100, 2) + torch.tensor([2.0, 2.0])    # classe 1 — top right
X = torch.cat([X0, X1])
y = torch.cat([torch.zeros(100), torch.ones(100)]).long()

# --- Model: two-layer MLP, input 2D -> hidden 8 -> output 2 classes ---
model = nn.Sequential(
    nn.Linear(2, 8),
    nn.ReLU(),
    nn.Linear(8, 2)
)

# --- Training ---
optimizer = torch.optim.Adam(model.parameters(), lr=0.01)
loss_fn = nn.CrossEntropyLoss()

for epoch in range(200):
    optimizer.zero_grad()
    loss = loss_fn(model(X), y)
    loss.backward()
    optimizer.step()

print(f"Loss finale : {loss.item():.4f}")

# --- Export model as TorchScript for C++ inference via LibTorch ---
scripted = torch.jit.script(model)
scripted.save("model.pt")
print("Model exported : model.pt")

# --- Export dataset as CSV for C++ visualization ---
with open('../data/points.csv', 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['x', 'y', 'label'])
    for i in range(len(X)):
        writer.writerow([
            X[i][0].item(),
            X[i][1].item(),
            y[i].item()
        ])

print("Dataset exported : data/points.csv")
