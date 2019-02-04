# -*- coding: utf-8 -*-
__author__ = 'Saber'
__date__ = '2019/2/4 0004'


import tensorflow as tf
from time import time


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



