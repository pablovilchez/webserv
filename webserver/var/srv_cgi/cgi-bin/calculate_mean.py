import sys


def parse_arguments(query_string):
    params = query_string.split('&')
    parsed_args = []
    for param in params:
        try:
            parsed_args.append(int(param))
        except ValueError:
            pass
    return parsed_args

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Error: one argument is needed.")
        sys.exit(1)

    query_string = sys.argv[1]
    parsed_args = parse_arguments(query_string)

    if parsed_args:
        mean = sum(parsed_args) / len(parsed_args)
        print("The mean of the integer values is: ", int(mean))
    else:
        print("There are no integer values to calculate the mean.")
