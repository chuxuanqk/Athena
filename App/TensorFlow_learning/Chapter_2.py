# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '18/2/19 下午9:12'

import tensorflow as tf
from tensorflow import keras

import numpy as np


# IMDB数据集，互联网电影数据集 50000条影评文本
imdb = keras.datasets.imdb
(train_data, train_labels), (test_data, test_labels) = imdb.load_data(num_words=10000)



