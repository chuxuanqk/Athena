# -*- coding:utf-8 -*-
__author__ = 'Saber'
__date__ = '18/1/19 上午11:18'


##############################################################
#   标识(token)是具有一定的句法语义且独立的最小文本成分
#
#   文本切分技术：句子切分和词语切分
#   将文本语料库分解成句子，将每个句子分解成单词
#
#   文本切分：将文本数据分解或拆分为具有更小且有意义的成分(token)的过程
#   NLTK句子切分器(sent_tokenize、PunktSentenceTokenizer、RegexpTokenizer、预先训练的句子切分模型)
#
##############################################################


import nltk
from nltk.corpus import gutenberg     # (Gutenberg语料库)
from pprint import pprint


def Download_Corpus():
    """
    下载NLTK所需要的全部资源
    :return:
    """
    nltk.download('all')


if __name__ == '__main__':
    Download_Corpus()



















