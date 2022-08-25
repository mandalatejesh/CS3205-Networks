
import binascii
dict = {
    '0': '0000',
    '1': '0001',
    '2': '0010',
    '3': '0011',
    '4': '0100',
    '5': '0101',
    '6': '0110',
    '7': '0111',
    '8': '1000',
    '9': '1001',
    'A': '1010',
    'B': '1011',
    'C': '1100',
    'D': '1101',
    'E': '1110',
    'F': '1111'
}
def hex2bin(s):
    s = s.upper()
    return dict[s]

def getParity(i, string):
    if i < 1:
        print('Illegal invocation')
        return 0
    oneCount = 0
    j = i
    while j < len(string):
        for k in range(j,min(j+i, len(string)-1)):
            if string[k]=='1':
                oneCount = oneCount + 1
        j = j + 2*i
    return oneCount%2

ignoreBits = [0,1,2,4,8,16,32,39]

f = open('messagecode.txt','r')

total_msg = ''
recieved_string=''
flipped_indices = []
num_blocks = 0
for line in f:
    recieved_string = recieved_string + line
    for block in range(0,len(line),10):
        block_string = line[block:min(block+10,len(line))]
        num_blocks = num_blocks + 1
        binary_string = ''

        for i in range(len(block_string)):
                binary_string = binary_string + hex2bin(block_string[i])
        error_index = 0
        for i in range(5,-1,-1):
            error_index = error_index * 2
            error_index = error_index + getParity(2**i, binary_string)

        if error_index < 40:
            if error_index == 0:
                flipped_indices.append('-')
            else:
                if binary_string[error_index]=='1':
                    binary_string = binary_string[:error_index] + '0'+binary_string[error_index+1:]
                else:
                    binary_string = binary_string[:error_index] + '1'+binary_string[error_index+1:]
                flipped_indices.append(error_index)

        corrected_string = ''
        for i in range(40):
            if i not in ignoreBits:
                corrected_string = corrected_string + binary_string[i]

        binary_int = int(corrected_string, 2)
        print(corrected_string)

        # Getting the byte number
        byte_number = binary_int.bit_length() + 7 // 8

        # Getting an array of bytes
        binary_array = binary_int.to_bytes(byte_number, "big")

        # Converting the array into ASCII text
        ascii_text = binary_array.decode()

        # Getting the ASCII value
        total_msg = total_msg + ascii_text


f.close()
print('Text: '+total_msg)
print('CodeWord: '+recieved_string)
print('Bit flip idx: ',end='')
for i in flipped_indices:
    print(str(i)+' ',end='')
print('\nNum blocks: '+str(num_blocks))
