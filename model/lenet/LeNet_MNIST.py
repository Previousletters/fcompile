from random import shuffle
import torch
import torch.onnx
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torchvision import datasets, transforms
from torch.autograd import Variable
from quant_sparse_layers import QuanSparseConv2dSRP, QuanSparseLinearSRP

lr = 0.01
momentum = 0.5
log_interval = 10
epochs = 10
batch_size = 64
test_batch_size = 1000

conv_sparsity = 0.2
conv_bit_width = 8
fc_sparsity = 0.3
fc_bit_width = 8
export_onnx = 1


class LeNet(nn.Module):
    def __init__(self):
        super(LeNet, self).__init__()
        self.conv1 = nn.Sequential(
            QuanSparseConv2dSRP(1, 6, kernel_size=5, stride=1, padding=2, sparsity=conv_sparsity, bit_width=conv_bit_width),
            nn.ReLU(),
            nn.MaxPool2d(kernel_size=2, stride=2),
        )
        self.conv2 = nn.Sequential(
            QuanSparseConv2dSRP(6, 16, kernel_size=5, stride=1, padding=0, sparsity=conv_sparsity, bit_width=conv_bit_width),
            nn.ReLU(),
            nn.MaxPool2d(kernel_size=2, stride=2)
        )
        self.fc1 = nn.Sequential(
            QuanSparseLinearSRP(16 * 5 * 5, 120, sparsity=fc_sparsity, bit_width=fc_bit_width),
            nn.ReLU()
        )
        self.fc2 = nn.Sequential(
            QuanSparseLinearSRP(120, 84, sparsity=fc_sparsity, bit_width=fc_bit_width),
            nn.ReLU()
        )
        self.fc3 = QuanSparseLinearSRP(84, 10, sparsity=fc_sparsity, bit_width=fc_bit_width)

    def forward(self, x):
        x = self.conv1(x)
        x = self.conv2(x)
        x = x.view(x.size()[0], -1)
        x = self.fc1(x)
        x = self.fc2(x)
        x = self.fc3(x)
        # return F.softmax(x, dim=1)  # x
        return x


def train(epoch):
    model.train()
    for batch_idx, (data, target) in enumerate(train_loader):
        data = data.to(device)
        target = target.to(device)
        data, target = Variable(data), Variable(target)
        optimizer.zero_grad()
        output = model(data)
        loss = F.cross_entropy(output, target)
        loss.backward()
        optimizer.step()
        if batch_idx % log_interval == 0:
            print('Train Epoch: {} [{}/{} ({:.0f}%)]\tLoss: {:.6f}'.format(
                epoch, batch_idx * len(data), len(train_loader.dataset),
                    100. * batch_idx / len(train_loader), loss.item()))


def test():
    model.eval()
    test_loss = 0
    correct = 0
    for data, target in test_loader:
        data = data.to(device)
        target = target.to(device)
        data, target = Variable(data), Variable(target)

        output = model(data)
        test_loss += F.cross_entropy(output, target, reduction='sum').item()
        pred = output.data.max(1, keepdim=True)[1]
        correct += pred.eq(target.data.view_as(pred)).cpu().sum()

    test_loss /= len(test_loader.dataset)
    print('\nTest set: Average Loss: {:.4f}, Accuracy: {}/{} ({:.0f}%)\n'.format(
        test_loss, correct, len(test_loader.dataset),
        100. * correct / len(test_loader.dataset)))


if __name__ == '__main__':
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    train_loader = torch.utils.data.DataLoader(
        datasets.MNIST('../data', train=True, download=True,
                        transform=transforms.Compose([
                            transforms.ToTensor(),
                            transforms.Normalize((0.1307,), (0.3081,))
                        ])),
        batch_size=batch_size, shuffle=True)

    test_loader = torch.utils.data.DataLoader(
        datasets.MNIST('../data', train=False, 
                        transform=transforms.Compose([
                            transforms.ToTensor(),
                            transforms.Normalize((0.1307,), (0.3081,))
                        ])),
        batch_size=test_batch_size, shuffle=True)

    model = LeNet()
    model = model.to(device)
    optimizer = optim.SGD(model.parameters(), lr=lr, momentum=momentum)

    model.load_state_dict(torch.load('model.pth'))
    if export_onnx:
        dummy_input = torch.randn(1, 1, 28, 28).to(device)
        # model(dummy_input)
        model.eval()
        torch.onnx.export(model, 
                        dummy_input,
                        "model.onnx", 
                        input_names=["input"],
                        output_names=["output"],
                        )
    else:
        for epoch in range(1, epochs + 1):
            train(epoch)
            test()
        torch.save(model.state_dict(), 'model.pth')

