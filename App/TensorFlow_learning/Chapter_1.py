# -*- coding: utf-8 -*-
__author__ = 'Saber'
__date__ = '2019/2/4 0004'

from time import time
import numpy as np
import matplotlib.pyplot as plt

import tensorflow as tf
from tensorflow import keras


def Add_tensor():
    matrix1 = tf.constant([[3., 3.]])
    matrix2 = tf.constant([[2.], [2.]])

    product = tf.matmul(matrix1, matrix2)

    # 启动默认图
    start = time()
    with tf.Session() as sess:
        result = sess.run([product])
        print(result)

    end = time()
    print("waste:", (end-start))



###########################################
#
#        Tensorflow基本分类
#使用tf.keras, 对服饰图像进行分类的神经网络
##########################################
def fashion_data():
    fashion_mnist = keras.datasets.fashion_mnist

    (train_images, train_labels), (test_images, test_labels) = fashion_mnist.load_data()

    return fashion_mnist




















