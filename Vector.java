package util;

import java.util.Collection;
import java.util.Iterator;
import java.util.stream.Collector;


/**
 * A collection that internally uses a circular buffer to store elements. This makes it efficient at pushing to the
 * front and back, and popping from the front and back. It is also efficient at random access as well as checking the
 * size or if the collection is empty. All other operations are slow.
 * 
 * <p> <b><i> Basically just a list that also has efficient push and pop operations on both sides. </i></b> </p>
 * 
 * <ul>
 * Time complexities
 * <li> get, at, set, pushFront, pushBack, popFront, popBack, size, isEmpty, iterator, add, clear, : <b>O(1)</b>
 * <li> contains, toArray, remove, addAll, toString: <b>O(n)</b>
 * <li> removeAll, retainAll : <b>O(n)+</b>
 * <li> containsAll : <b>O(n^2)</b>
 * </ul>
 * 
 * <p> removeAll and retainAll are O(n) if the other collection has a O(1) contains method. Otherwise they'll be slower.
 * 
 * <p> {@code NOTE:} the last element in the collection can be accessed with get(-1) if desired!
 * 
 * @param <T> the type of elements in this list
 * 
 * 
 * @author Brandon Fowler
 * @see Collection
 */
public class Vector<T> implements Collection<T> {
    
    private T[] data = (T[]) new Object[10];
    private int startIndex = 0;
    private int endIndex = 0;

    /**
     * Create an empty vector.
     */
    public Vector () {}

    /**
     * Create a vector of 'size' with 'size' null elements.
     * @param size
     */
    public Vector(int size) {
        this.data = (T[]) new Object[size * 2];
        this.startIndex = 0;
        this.endIndex = size;
    }

    public Vector(Collection<T> collection) {
        this.addAll(collection);
    }

    @SafeVarargs
    public Vector(T ... elements) {
        for (T element : elements) {
            this.add(element);
        }
    }

    public Vector<T> of(T ... elements) {
        return new Vector<>(elements);
    }

    /**
     * Get the element at the specified index. O(1)
     * @param index
     * @return
     */
    public T get(int index) {
        checkOutOfBounds(index);
        int realIndex = convertIndex(index);

        return data[realIndex];
    }

    /**
     * Get the element at the specified index. O(1)
     * @param index
     * @return
     */
    public T at(int index) {
        return this.get(index);
    }

    /**
     * Set the element at the specified index. O(1)
     * @param value
     * @param index
     */
    public void set(int index, T value) {
        checkOutOfBounds(index);
        int realIndex = convertIndex(index);

        data[realIndex] = value;
    }

    /**
     * Push an element to the front of the vector. O(1) amortized.
     * @param value
     */
    public void pushFront(T value) {
        if (size() == data.length - 1) {
            resize();
        }

        startIndex = (startIndex - 1 + data.length) % data.length;
        data[startIndex] = value;
    }

    /**
     * Push an element to the back of the vector. O(1) amortized.
     * @param value
     */
    public void pushBack(T value) {
        if (size() == data.length - 1) {
            resize();
        }

        data[endIndex] = value;
        endIndex = (endIndex + 1) % data.length;
    }

    /**
     * Pop an element from the front of the vector. O(1) amortized.
     * @return
     */
    public T popFront() {
        T element = data[startIndex];
        data[startIndex] = null;
        startIndex = (startIndex + 1) % data.length;

        if (size() < data.length / 4) {
            shrink();
        }

        return element;
    }

    /**
     * Pop an element from the back of the vector. O(1) amortized.
     * @return
     */
    public T popBack() {
        int lastEnd = (endIndex - 1 + data.length) % data.length;
        T element = data[lastEnd];
        data[lastEnd] = null;   
        endIndex = lastEnd;

        if (size() < data.length / 4) {
            shrink();
        }

        return element;
    }


    private void resize() {
        int size = size();
        T[] newData = (T[]) new Object[data.length * 2];
        if (startIndex > endIndex) {
            int startSize = this.data.length - startIndex;
            System.arraycopy(this.data, startIndex, newData, 0, startSize);
            System.arraycopy(this.data, 0, newData, startSize, endIndex);
        }
        else if (startIndex == endIndex) {
            // do nothing empty array
        }
        else {
            System.arraycopy(this.data, startIndex, newData, 0, size);
        }

        data = newData;
        startIndex = 0;
        endIndex = size;
    }

    private void shrink() {
        T[] newData = (T[]) new Object[data.length / 2];
        int size = size();
        for (int i = 0; i < size; i++) {
            newData[i] = get(i);
        }
        data = newData;
        startIndex = 0;
        endIndex = size;
    }

    private int convertIndex(int index) {

        if (index < 0) {
            index += size();
        }

        return (startIndex + index) % data.length;
    }

    private void checkOutOfBounds(int index) {

        if (index < 0) {
            index += size();
        }

        boolean inBounds = index >= 0 && index < size();

        if (!inBounds) {
            throw new IndexOutOfBoundsException("Index: " + index + ", Size: " + size());
        }
    }


    @Override
    public int size() {
        if (startIndex > endIndex) {
            return data.length - startIndex + endIndex;
        }
        else if (startIndex == endIndex) {
            return 0;
        }
        else {
            return endIndex - startIndex;
        }
    }

    @Override
    public boolean isEmpty() {
        return startIndex == endIndex;
    }

    @Override
    public boolean contains(Object o) {

        for (int i = startIndex; i != endIndex; i++) {
            if (i >= data.length) i = 0;
            if (data[i].equals(o)) {
                return true;
            }
        }

        return false;
    }

    @Override
    public Iterator<T> iterator() {
        return new Iterator<T>() {
            int currentIndex = startIndex;

            @Override
            public boolean hasNext() {
                return currentIndex != endIndex;
            }

            @Override
            public T next() {
                
                T value = data[currentIndex];
                currentIndex = (currentIndex + 1) % data.length;
                return value;
            }
            
        };
    }

    @Override
    public Object[] toArray() {
        return this.toArray(new Object[size()]);
    }

    @Override
    public <T> T[] toArray(T[] a) {
        int size = this.size();
        T[] array = a.length >= size ? a : (T[]) new Object[size];

        Iterator<T> iterator = (Iterator<T>) this.iterator();
        int currentIndex = 0;
        while(iterator.hasNext()) {
            array[currentIndex] = iterator.next();
            currentIndex++;
        }
        return array;
    }

    @Override
    public boolean add(T e) {
        this.pushBack(e);
        return true;
    }

    @Override
    public boolean remove(Object o) {
        T[] newData = (T[]) new Object[data.length];
        
        boolean wasFound = false;
        int currentIndex = 0;
        for (int i = startIndex; i != endIndex; i++) {
            if (i >= data.length) i = 0;
            if (data[i] != null && !data[i].equals(o)) {
                newData[currentIndex] = data[i];
                currentIndex++;
            }
            else {
                wasFound = true;
            }
        }

        this.data = newData;
        this.startIndex = 0;
        this.endIndex = currentIndex;

        return wasFound;
    }

    @Override
    public boolean containsAll(Collection<?> c) {
        for (Object o : c) {
            if (!this.contains(o)) {
                return false;
            }
        }
        return true;
    }

    @Override
    public boolean addAll(Collection<? extends T> c) {
        for (T t : c) {
            this.add(t);
        }
        return c.size() > 0;
    }

    @Override
    public boolean removeAll(Collection<?> c) {

        T[] newData = (T[]) new Object[data.length];
        int currentIndex = 0;

        boolean wasChanged = false;
        for (int i = startIndex; i != endIndex; i++) {
            if (i >= data.length) i = 0;
            if (!c.contains(data[i])) {
                newData[currentIndex] = data[i];
                currentIndex++;
            }
            else {
                wasChanged = true;
            }
        }

        if (wasChanged) {
            this.data = newData;
            this.startIndex = 0;
            this.endIndex = currentIndex;
        }

        return wasChanged;
    }

    @Override
    public boolean retainAll(Collection<?> c) {

        T[] newData = (T[]) new Object[data.length];
        int currentIndex = 0;

        boolean wasChanged = false;
        for (int i = startIndex; i != endIndex; i++) {
            if (i >= data.length) i = 0;
            if (c.contains(data[i])) {
                newData[currentIndex] = data[i];
                currentIndex++;
            }
            else {
                wasChanged = true;
            }
        }

        if (wasChanged) {
            this.data = newData;
            this.startIndex = 0;
            this.endIndex = currentIndex;
        }

        return wasChanged;
    }

    @Override
    public void clear() {
        this.data = (T[]) new Object[10];
        this.startIndex = 0;
        this.endIndex = 0;
    }


    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder("[\n");
        for (T t : this) {
            sb.append(" ");
            sb.append(t);
            sb.append("\n");
        }
        sb.append("]");
        return sb.toString();
    }

    public static <T> Collector<T, Vector<T>, Vector<T>> collector() {
        return Collector.of(
            Vector::new,
            Vector::pushBack,
            (left, right) -> {
                left.addAll(right);
                return left;
            }
        );
    }


    // Tests
    public static void main(String[] args) {
        try {
            Vector<Integer> list = new Vector<>();
            list.pushBack(1);
            list.pushBack(2);
            list.pushBack(3);
            list.pushFront(0);
            assertEquals(0, list.get(0));
            assertEquals(1, list.get(1));
            assertEquals(2, list.get(2));
            assertEquals(3, list.get(3));
            assertEquals(4, list.size());
    
            list.pushBack(5);
            assertEquals(5, list.size());

            assertEquals(false, list.isEmpty());
            list.remove(list.get(0));
            list.remove(list.get(0));
            list.remove(list.get(0));
            list.remove(list.get(0));
            list.remove(list.get(0));

            assertEquals(true, list.isEmpty());
            assertEquals(false, list.contains(0));
            list.add(0);
            assertEquals(true, list.contains(0));

            list.add(1);
            list.add(2);
            list.add(3);
            list.add(4);
            list.add(5);
            list.add(6);
            list.add(7);
            list.add(8);
            int index = 0;
            for (Integer i : list) {
                assertEquals(index, i);
                index++;
            }
            assertEquals(8, list.get(-1));

            Integer[] array = new Integer[list.size()];
            list.toArray(array);
            assertEquals(7, array[7]);

            Object[] objArray = list.toArray();
            assertEquals(1, objArray[1]);

            Vector<Integer> list2 = new Vector<>();
            list2.add(0);
            assertEquals(true, list.containsAll(list2));

            Vector<Integer> list3 = new Vector<>();
            list3.add(9);
            list.addAll(list3);
            assertEquals(9, list.get(9));

            list.removeAll(list3);
            assertEquals(9, list.size());

            list.retainAll(list2);
            assertEquals(1, list.size());
            assertEquals(0, list.get(0));

            list.set(0, 1);
            assertEquals(1, list.at(0));

            list.clear();
            assertEquals(0, list.size());


            for (int i = 0; i < 100; i++) {
                list.pushBack(i);
                list.pushFront(i);
            }
            System.out.println(list.toString());

            assertEquals(99, list.popFront());
            assertEquals(99, list.popBack());
            assertEquals(198, list.size());

            for (int i = 0; i < 79; i++) {
                list.popFront();
                list.popBack();
            }
            assertEquals(40, list.size());
            assertEquals(19, list.get(0));
            assertEquals(19, list.get(-1));

            list = new Vector<>(4);
            assertEquals(4, list.size());
            list.pushBack(1);
            assertEquals(5, list.size());
            assertEquals(null, list.at(0));

            System.out.println("TESTS PASSED");
        }
        catch (Throwable e) {
            e.printStackTrace();
        }
    }

    private static void assertEquals(Object expected, Object actual) {

        boolean checkFailed = false;

        // check if expected is a primitive type or null
        if (expected == null || expected.getClass().isPrimitive()) {
            checkFailed = expected != actual;
        }
        else {
            checkFailed = !expected.equals(actual);
        }

        if (checkFailed) {
            throw new AssertionError("Expected " + expected + " but got " + actual);
        }
    }
}
