import java.io.Serializable;

class Main implements Serializable{
    private static int z = 0;
    
    public static int add(int x, int y){
        return x + y;
    }

    public static void main(String[] args) {
        int value = add(22, 20);
        System.out.print(value);
    }
}