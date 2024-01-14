import subprocess

VGG_MODEL_URL          = 'https://s3.amazonaws.com/onnx-model-zoo/vgg/vgg16/vgg16.onnx'
VGG_LBL_URL            = 'https://s3.amazonaws.com/onnx-model-zoo/synset.txt'
VGG_IMG_URL            = 'https://s3.amazonaws.com/model-server/inputs/kitten.jpg'
VGG_ONNX_FILENAME      = 'vgg16.onnx'
VGG_LBL_FILENAME       = 'synset.txt'
RAW_LIST_FILE          = 'raw_list.txt'

def wget(download_dir, file_url):
    cmd = ['wget', '-N', '--directory-prefix={}'.format(download_dir), file_url]
    subprocess.call(cmd)

def download():
    wget("onnx", VGG_MODEL_URL)


if __name__ == "__main__":
    download()
