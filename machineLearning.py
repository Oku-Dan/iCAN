import matplotlib.pyplot as plt

def plot_history(history):
  acc = history.history['acc']
  epochs = range(len(acc))
  plt.plot(epochs, acc, 'bo' ,label = 'training acc')
  plt.title('Model accuracy')
  plt.ylabel('Accuracy')
  plt.xlabel('Epoch')
  plt.show()

def output_model(model):
  # for layer in range(len(model.layers)):
  #   weights = model.layers[layer].get_weights()
  #   for weight in range(len(weights[0])):
  #     print('Layer:' + str(layer) + ' Node : ' + str(weight),end="[")
  #     for w in weights[0][weight]:
  #       print(w,end=',')
  #     print(']')
  #   print('Layer:' + str(layer) + ' Bias',end="[")
  #   for w in weights[1]:
  #     print(w,end=',')
  #   print(']')
  # print(len(model.layers)) #層数
  # for layer in model.layers:
  #   weights = layer.get_weights()
  #   print(len(weights[0])) #ある層のノード数
  #   print(len(weights[0][0])) #前の層のノード数
  #   for weight in weights[0]:
  #     for val in weight:
  #       print(val,end=" ")
  #     print()
  #   for bias in weights[1]:
  #     print(bias,end=" ")
    # print()
  print(len(model.layers))
  for layer in model.layers:
    weights = layer.get_weights()
    for i in range(len(weights[0][0])):
      for j in range(len(weights[0])):
        print('{:.4f}'.format(weights[0][j][i]),end=",")
      print()
  print()
  for layer in model.layers:
    weights = layer.get_weights()
    for bias in weights[1]:
      print('{:.4f}'.format(bias),end=",")
    print()
import numpy as np
from keras.models import Sequential
from keras.layers import Activation, Dense
from keras.datasets import mnist
from keras.utils.np_utils import to_categorical

# (x_train, y_train), (x_test, y_test) = mnist.load_data()

with open("trainHELLO.txt") as file:
    lines = file.readlines()

x_train = []
y_train = []
for line in lines:
  vals = line.split(' ')
  temp = []
  for val in vals[:-1]:
    temp.append(float(val))
  x_train.append(temp)
  y_train.append(int(vals[-1]))
x_train = np.array(x_train)
print(x_train[0])
# x_train = x_train.reshape((60000, 28 * 28))
# x_train = x_train.astype('float32') / 255
# x_test = x_test.reshape((10000, 28 * 28))
# x_test = x_test.astype('float32') / 255

y_train = to_categorical(y_train,4)
# print(y_train)
# y_test = to_categorical(y_test,10)

model = Sequential()
model.add(Dense(32,activation='relu',input_shape=(len(x_train[0]),)))
model.add(Dense(32,activation='relu'))
model.add(Dense(4,activation='softmax'))
model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=["accuracy"])

model.summary()

history = model.fit(x_train,y_train,epochs=15)
plot_history(history)

# from keras.models import load_model
# model = load_model('keras_model.h5')

# score = model.evaluate(x_test, y_test)
# print('accuracy=', score[1])

# model.save('keras_model.h5')
# model.save_weights('keras_model_weights.h5')

output_model(model)