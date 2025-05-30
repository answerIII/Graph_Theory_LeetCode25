import matplotlib.pyplot as plt

from definitions import (
    PROJECT_ROOT_DIR,
    IMAGES_VERTEX_LWC_RATIO_RANDOM,
    IMAGES_VERTEX_LWC_RATIO_BY_DEGREE
)

if __name__ == "__main__":
    # parse results
    NUMBER_OF_DATASETS = 13
    RANGE_SIZE = 20
    with open(PROJECT_ROOT_DIR + '/results/1B.txt', mode="r") as input_file:
        for _ in range(NUMBER_OF_DATASETS):
            first_line = input_file.readline()
            image_name = first_line[first_line.rfind('/') + 1 : first_line.rfind('.')] + '.png'
            percentage = [0] + list(range(1, 100, 5))
            original_ratio = float(input_file.readline().split(': ')[1].strip())
            ratio = [original_ratio] + [0] * RANGE_SIZE
            for i in range(RANGE_SIZE):
                ratio[i + 1] = float(input_file.readline().split(': ')[1].strip())
            plt.plot(percentage, ratio)
            plt.title('Delete x% of random vertexes')
            plt.xlabel('x')
            plt.ylabel('LWC ratio')
            plt.savefig(IMAGES_VERTEX_LWC_RATIO_RANDOM + image_name)
            plt.close()
            for i in range(RANGE_SIZE):
                ratio[i + 1] = float(input_file.readline().split(': ')[1].strip())
            plt.plot(percentage, ratio)
            plt.title('Delete x% of vertexes sorted by degree')
            plt.xlabel('x')
            plt.ylabel('LWC ratio')
            plt.savefig(IMAGES_VERTEX_LWC_RATIO_BY_DEGREE + image_name)
            plt.close()