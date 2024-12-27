import yaml
import os
from colorama import init, Fore,Style

init(autoreset=True)

def print_aligned_dict(d, indent=0):
    max_key_length = max(len(str(key)) for key in d.keys())
    for key, value in d.items():
        if isinstance(value, dict):
            print(' ' * indent + f"{key}:")
            print_aligned_dict(value, indent + 4)
        else:
            print(' ' * indent + f"{key:{max_key_length}}: {value}")


def per_test(test_file, ans_data, out_data):
    if isinstance(ans_data, list) and isinstance(out_data, list):
        for i in range(len(ans_data)):
            if isinstance(ans_data[i], dict) and isinstance(out_data[i], dict):
                for key, value in ans_data[i].items():
                    if key in out_data[i]:
                        if value != out_data[i][key]:
                            print(f"{test_file} test {Fore.RED}failed{Style.RESET_ALL}.")
                            print(f"Test aborted at PC = {out_data[i]['PC']}")
                            print(f"Expected: {key}:")
                            if isinstance(value, dict):
                                print_aligned_dict(value, 4)
                            else:
                                print(f"    {key}: {value}")
                            print(f"Output: {key}:")
                            if isinstance(out_data[i][key], dict):
                                print_aligned_dict(out_data[i][key], 4)
                            else:
                                print(f"    {key}: {out_data[i][key]}")

                            return False
                    else:
                        print(f"Output file {test_file} may contain formatting errors.")
                        print(f"{test_file} test {Fore.RED}aborted{Style.RESET_ALL}.")
                        return False
            else:
                print(f"Output file {test_file} may contain formatting errors.")
                print(f"{test_file} test {Fore.RED}aborted{Style.RESET_ALL}.")
                return False
        return True
    else:
        print(f"Output file {test_file}: Unsupported format")
        print(f"{test_file} test {Fore.RED}aborted{Style.RESET_ALL}.")
        return False


def cpu_test():
    """read all files in the output and answer directories"""
    output_dir = './output'
    answer_dir = './answer'
    ans_files = [f for f in os.listdir(answer_dir) if os.path.isfile(os.path.join(answer_dir, f))]
    out_files = [f for f in os.listdir(output_dir) if os.path.isfile(os.path.join(output_dir, f))]

    common_files = set(ans_files).intersection(out_files)
    missing_files = set(ans_files) - common_files
    all_passed = True

    """test each file"""
    for file_name in common_files:
        try:
            with open(os.path.join(answer_dir, file_name), 'r') as file:
                ans_data = yaml.safe_load(file)
            with open(os.path.join(output_dir, file_name), 'r') as file:
                out_data = yaml.safe_load(file)
        except yaml.YAMLError as e:
            print(f"{Fore.RED}Error parsing YAML file {file_name}{Style.RESET_ALL}:\n {e}")
            all_passed = False
            break

        if per_test(file_name, ans_data, out_data):
            print(f"{file_name} test {Fore.GREEN}passed{Style.RESET_ALL}.")
        else:
            all_passed = False
            break
    
    """check for missing files"""
    if missing_files:
        print(f"\n{Fore.YELLOW}The following tests are missing:")
        for missing_file in missing_files:
            print(f"{missing_file}")
        all_passed = False 

    return all_passed


if __name__ == "__main__":
    if cpu_test():
        print(f"{Fore.GREEN}All tests passed.")